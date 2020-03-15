//
//  Sequencer.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/12.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef Sequencer_h
#define Sequencer_h

#include "MidiFile.h"
#include "MidiSetting.h"

#include <iostream>
#include <functional>
#include <initializer_list>
#include <vector>

namespace sequencer {
    using RampFn = std::function<float(float)>;
    using OscFn  = std::function<float(float)>;
    using CondFn = std::function<bool(float, float)>;
    using Notes  = std::vector<int>;
    
    static std::function<bool(float, float)> notePerDiv(int div) {
        float diviser = 1.0 / div;
        return [div, diviser](float from, float to) -> bool {
            for(auto i = 0; i < div; ++i) {
                const float p = i * diviser;
                if(from <= p && p < to) return true;
            }
            return false;
        };
    }
    
    struct PlayMode {
        enum class play_mode : std::uint8_t {
            repeat,
            reverse,
            pingpong
        };

        static PlayMode Linear(std::size_t repeat_num)
        { return {play_mode::repeat, repeat_num}; };
        
        static PlayMode Repeat(std::size_t repeat_num)
        { return {play_mode::repeat, repeat_num}; };
        
        static PlayMode Reverse(std::size_t repeat_num)
        { return {play_mode::reverse, repeat_num}; };
        
        static PlayMode Pingpong(std::size_t repeat_num)
        { return {play_mode::pingpong, repeat_num}; };
        
        PlayMode() = default;
        PlayMode(play_mode mode, std::size_t repeat_num)
        : mode{mode}
        , repeat_num{repeat_num}
        {};
        PlayMode(const PlayMode &) = default;
        PlayMode(PlayMode &&) = default;
        
        play_mode mode;
        std::size_t repeat_num{1};
    };

    struct AbstractPhrase {
        struct EvaluateParameter {
            float from;
            float to;
            int tpq;
            int tick;
            int bar_tick;
            int elapsed_bar;
        };
        virtual void evaluate(smf::MidiFile &file,
                              int track_id,
                              const EvaluateParameter &parameter) const = 0;
    };
    
    using PhraseEvaluateParameter = AbstractPhrase::EvaluateParameter;
    
    struct GeneratablePhrase : AbstractPhrase {
        GeneratablePhrase() = default;
        GeneratablePhrase(const Notes &notes, RampFn ramp, CondFn cond)
        : notes{notes}
        , ramp{ramp}
        , cond{cond}
        {};
        
        GeneratablePhrase(const Notes &notes, RampFn ramp, int div)
        : GeneratablePhrase{notes, ramp, notePerDiv(div)}
        {};
        
        Notes notes;
        RampFn ramp;
        CondFn cond;
        
        void evaluate(smf::MidiFile &file,
                      int track_id,
                      const EvaluateParameter &parameter) const
        {
            if(cond(parameter.from, parameter.to)) {
                int note_id = math::min(notes.size() * ramp(parameter.from), notes.size() - 1);
                int key = notes[note_id];
                file.addNoteOn(track_id,
                               parameter.elapsed_bar * parameter.bar_tick + parameter.tick,
                               0,
                               key,
                               100);
                file.addNoteOff(track_id,
                                parameter.elapsed_bar * parameter.bar_tick + parameter.tick + 30,
                                0,
                                key);
            }

        }
    };
    
    struct Sequence {
        struct playable_phrase {
            playable_phrase() = default;
            playable_phrase(const playable_phrase &) = default;
            playable_phrase(playable_phrase &&) = default;
            
            template <typename Phrase, typename std::enable_if<std::is_base_of<AbstractPhrase, Phrase>::value> * = nullptr>
            playable_phrase(PlayMode &&playmode, const Phrase &phrase)
            : playmode{std::move(playmode)}
            , phrase{std::make_shared<Phrase>(phrase)}
            {};
            
            PlayMode playmode;
            std::shared_ptr<AbstractPhrase> phrase;
        };
        
        template <typename Phrase>
        auto play(PlayMode &&playmode, const Phrase &phrase)
            -> typename std::enable_if<std::is_base_of<AbstractPhrase, Phrase>::value, Sequence &>::type
        {
            phrases.emplace_back(std::move(playmode), phrase);
            return *this;
        }
        
        template <typename cond_t>
        Sequence &play(PlayMode &&playmode, const std::vector<int> &notes, RampFn ramp, cond_t cond) {
            return play(std::move(playmode), GeneratablePhrase{notes, ramp, cond});
        }
        
        template <typename Phrase>
        auto then(PlayMode &&playmode, const Phrase &phrase)
            -> typename std::enable_if<std::is_base_of<AbstractPhrase, Phrase>::value, Sequence &>::type
        {
            return play(std::move(playmode), phrase);
        }
        
        template <typename cond_t>
        Sequence &then(PlayMode &&playmode, const std::vector<int> &notes, RampFn ramp, cond_t cond) {
            return play(std::move(playmode), GeneratablePhrase{notes, ramp, cond});
        }
        
        void write(smf::MidiFile &file, int tpq = 120, int track_id = -1) {
            if(track_id == -1) {
                track_id = file.addTrack();
            }
            int elapsed_bar = 0;
            int bar_tick = tpq * 4;
            for(const auto &phrase_pair : phrases) {
                const auto &playmode = phrase_pair.playmode;
                const auto &phrase = phrase_pair.phrase;
                for(auto i = 0; i < playmode.repeat_num; ++i) {
                    for(auto tick = 0; tick < bar_tick; ++tick) {
                        PhraseEvaluateParameter p;
                        p.tick = tick;
                        p.bar_tick = bar_tick;
                        p.elapsed_bar = elapsed_bar;
                        p.tpq = tpq;
                        
                        if(playmode.mode == PlayMode::play_mode::reverse
                           || (
                               (playmode.mode == PlayMode::play_mode::pingpong)
                               && (i % 2 == 1)
                           )
                        ) {
                            p.from = (float)(bar_tick - tick - 1) / bar_tick;
                            p.to = (float)(bar_tick - tick) / bar_tick;
                        } else {
                            p.from = (float)tick / bar_tick;
                            p.to = (float)(tick + 1) / bar_tick;
                        }
                        
                        phrase->evaluate(file, track_id, p);
                    }
                    
                    elapsed_bar++;
                }
            }
            file.sortTracks();
        }
        
        inline void write(smf::MidiFile &file, MIDI::Setting setting)
        { write(file, setting.duration_in_ticks / 4, setting.track_id); };
        
        std::vector<playable_phrase> phrases;
    };
};


#endif /* Sequencer_h */

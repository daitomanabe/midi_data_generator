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
    
    struct Phrase {
        Phrase() = default;
        Phrase(const Notes &notes, RampFn ramp, CondFn cond)
        : notes{notes}
        , ramp{ramp}
        , cond{cond}
        {};
        
        Phrase(const Notes &notes, RampFn ramp, int div)
        : Phrase{notes, ramp, notePerDiv(div)}
        {};
        
        Notes notes;
        RampFn ramp;
        CondFn cond;
    };
    
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
    
    struct Sequence {
        struct playable_phrase {
            playable_phrase() = default;
            playable_phrase(const playable_phrase &) = default;
            playable_phrase(playable_phrase &&) = default;
            playable_phrase(PlayMode &&playmode, const Phrase &phrase)
            : playmode{std::move(playmode)}
            , phrase{phrase}
            {};
            
            PlayMode playmode;
            Phrase phrase;
        };
        
        Sequence &play(PlayMode &&playmode, const Phrase &phrase) {
            phrases.emplace_back(std::move(playmode), phrase);
            return *this;
        }
        template <typename cond_t>
        Sequence &play(PlayMode &&playmode, const std::vector<int> &notes, RampFn ramp, cond_t cond) {
            return play(std::move(playmode), Phrase{notes, ramp, cond});
        }
        
        Sequence &then(PlayMode &&playmode, const Phrase &phrase) {
            return play(std::move(playmode), phrase);
        }
        template <typename cond_t>
        Sequence &then(PlayMode &&playmode, const std::vector<int> &notes, RampFn ramp, cond_t cond) {
            return play(std::move(playmode), Phrase{notes, ramp, cond});
        }
        
        void write(smf::MidiFile &file, int tpq = 120, int track_id = -1) {
            if(track_id == -1) {
                track_id = file.addTrack();
            }
            std::size_t elapsed_bar = 0;
            int bar_tick = tpq * 4;
            for(const auto &phrase_pair : phrases) {
                const auto &playmode = phrase_pair.playmode;
                const auto &phrase = phrase_pair.phrase;
                for(auto i = 0; i < playmode.repeat_num; ++i) {
                    for(auto tick = 0; tick < bar_tick; ++tick) {
                        float from = 0.0f, to = 0.0f;
                        if(playmode.mode == PlayMode::play_mode::reverse
                           || (
                               (playmode.mode == PlayMode::play_mode::pingpong)
                               && (i % 2 == 1)
                           )
                        )
                        {
                            from = (float)(bar_tick - tick - 1) / bar_tick;
                            to = (float)(bar_tick - tick) / bar_tick;
                        } else {
                            from = (float)tick / bar_tick;
                            to = (float)(tick + 1) / bar_tick;
                        }
                        if(phrase.cond(from, to)) {
                            int note_id = math::min(phrase.notes.size() * phrase.ramp(from), phrase.notes.size() - 1);
                            int key = phrase.notes[note_id];
                            file.addNoteOn(track_id,
                                           elapsed_bar * bar_tick + tick,
                                           0,
                                           key,
                                           100);
                            file.addNoteOff(track_id,
                                            elapsed_bar * bar_tick + tick + 30,
                                            0,
                                            key);
                        }
                    }
                    
                    elapsed_bar++;
                }
            }
            file.sortTracks();
        }
        
        inline void write(smf::MidiFile &file, MIDI::Setting setting = {})
        { write(file, setting.duration_in_ticks / 4, setting.track_id); };
        
        std::vector<playable_phrase> phrases;
    };
};


#endif /* Sequencer_h */

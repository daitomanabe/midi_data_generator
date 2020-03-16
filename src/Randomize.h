//
//  Randomize.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/10.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef Randomize_h
#define Randomize_h

#include "MidiConstants.h"
#include "MidiSetting.h"

#include "MathConstants.h"
#include "MathUtils.h"

#include "MidiFile.h"
#include "SimplexNoise.h"

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

#include <cstddef>
#include <cstdint>

namespace randomize {
    namespace detail {
        struct pitch_id {
            pitch_id() = default;
            pitch_id(std::uint8_t channel, std::uint8_t pitch)
            : channel(channel)
            , pitch(pitch)
            {};
            
            std::uint8_t channel;
            std::uint8_t pitch;
            
            inline std::uint16_t hash() const
            { return channel * 128 + pitch; };
            
            inline bool operator<(const pitch_id &rhs) const
            { return hash() < rhs.hash(); };
            inline bool operator==(const pitch_id &rhs) const
            { return hash() == rhs.hash(); };
        };
        
        struct midi_note {
            std::uint8_t track_id;
            std::uint8_t channel;
            std::uint8_t pitch;
            std::uint8_t velocity;
            
            int tick;
            bool is_on;
        };
    };
    
    inline static void swap_pitches(smf::MidiFile &file,
                                    int track_id = -1)
    {
        if(track_id < 0) {
            for(auto i = 0; i < file.getNumTracks(); ++i) swap_pitches(file, i);
            return;
        } else if(file.getNumTracks() < track_id) {
            std::cerr << "given track_id is out of bounds: track_id must be less than " << file.getNumTracks() << " but " << track_id << " is given." << std::endl;
        }
        auto original = file;
        file[track_id].clear();
        
        const auto &event_list = original[track_id];
        
        std::map<detail::pitch_id, std::vector<detail::midi_note>> separated_notes;
        std::set<std::uint8_t> pitch_set;
        for(auto i = 0; i < event_list.size(); ++i) {
            const auto &event = event_list[i];
            if(event.isNote()) {
                auto &notes = separated_notes[detail::pitch_id(event.getChannel(), event.getKeyNumber())];
                notes.emplace_back();
                detail::midi_note &note = notes.back();
                note.track_id = event.track;
                note.channel = event.getChannel();
                note.pitch = event.getKeyNumber();
                note.velocity = event.getVelocity();
                note.is_on = event.isNoteOn();
                note.tick = event.tick;
                pitch_set.insert(note.pitch);
            } else {
                auto new_event = event;
                file[track_id].append(new_event);
            }
        }
        
        std::vector<std::uint8_t> pitches;
        pitches.reserve(pitch_set.size());
        std::copy(pitch_set.cbegin(),
                  pitch_set.cend(),
                  std::back_inserter(pitches));
        
        for(auto &pair : separated_notes) {
            auto &notes = pair.second;
            std::sort(notes.begin(), notes.end(),
                      [](const detail::midi_note &x,
                         const detail::midi_note &y)
                      { return x.tick < y.tick; });
            for(std::size_t i = 0; i < notes.size(); ++i) {
                const auto &note = notes[i];
                auto pitch = pitches[(std::size_t)math::random(0.0, pitches.size())];
                if(note.is_on) {
                    file.addNoteOn(track_id,
                                   note.tick,
                                   note.channel,
                                   pitch,
                                   note.velocity);
                    if(i + i < notes.size() && !notes[i + 1].is_on) {
                        file.addNoteOff(track_id,
                                        notes[i + 1].tick,
                                        notes[i + 1].channel,
                                        pitch);
                        i++;
                    }
                }
            }
        }
        
        file.sortTrack(track_id);
    }
    
    inline static void fill_by_perlin(smf::MidiFile &file,
                                      int track_id,
                                      const std::vector<int> &notes,
                                      float threashold,
                                      int div,
                                      int duration_in_ticks = MIDI::ONEMEASURE)
    {
        float unit = 1.0f / div;
        SimplexNoise simplex;
        for(auto note : notes) {
            for(auto i = 0; i < div; ++i) {
                float p = i * unit;
                float v = 0.5f * (simplex.noise(note, i) + 1.0f);
                if(threashold < v) {
                    file.addNoteOn(track_id,
                                   duration_in_ticks * p,
                                   0,
                                   note,
                                   127 * v);
                    file.addNoteOff(track_id,
                                    duration_in_ticks * (p + unit),
                                    0,
                                    note);
                }
            }
        }
        file.sortTracks();
    }
    
    inline static void arpeggiate_by_perlin(smf::MidiFile &file,
                                            int track_id,
                                            const std::vector<int> &notes,
                                            int div,
                                            int duration_in_ticks = MIDI::ONEMEASURE)
    {
        float unit = 1.0f / div;
        SimplexNoise simplex;
        for(auto i = 0; i < div; ++i) {
            std::vector<float> vs;
            vs.resize(notes.size());
            std::transform(notes.begin(), notes.end(), vs.begin(), [i, &simplex](int note) {
                return 0.5f * (simplex.noise(note, i) + 1.0f);
            });
            float p = i * unit;
            auto it = std::max_element(vs.begin(), vs.end());
            auto note = notes[std::distance(vs.begin(), it)];
            float v = *it;
            file.addNoteOn(track_id,
                           duration_in_ticks * p,
                           0,
                           note,
                           127 * v);
            file.addNoteOff(track_id,
                            duration_in_ticks * (p + unit),
                            0,
                            note);
        }
        file.sortTracks();
    }
    
    struct PhraseStuttingSetting : MIDI::Setting {
        int divisor{16};
        struct {
            MIDI::Probability probability{1.0f}; // 0.0f - 1.0f
        } random_note;
        struct {
            MIDI::Probability probability{0.0f}; // 0.0f - 1.0f
        } dropout;
        struct {
            MIDI::Probability probability{0.0f}; // 0.0f - 1.0f
            int time{0};
            int interval{MIDI::EIGHTH}; // in ticks
            struct {
                int min{0};
                int max{0};
            } decrease_interval;
        } repeat;
    };
    
    inline static void make_phrase(smf::MidiFile &file,
                                   const std::vector<int> &notes,
                                   const PhraseStuttingSetting &setting)
    {
        int unit = setting.duration_in_ticks / setting.divisor;
        const auto &repeat = setting.repeat;
        float normalizer = 1.0f / setting.duration_in_ticks;
        for(auto i = 0; i < setting.divisor; ++i) {
            if(setting.dropout.probability(i * unit * normalizer) < math::random()) {
                int position = setting.offset_in_ticks + i * unit;
                float normalized_position = position * normalizer;
                int duration = repeat.interval;
                int n = notes.size() * math::random() * setting.random_note.probability(normalized_position);
                int note = notes[n];
                file.addNoteOn(setting.track_id,
                               position,
                               0,
                               note,
                               100);
                file.addNoteOff(setting.track_id,
                                position + duration,
                                0,
                                note);
                if(0 < repeat.time && math::random() < repeat.probability(normalized_position))
                {
                    int repeat_decrease_interval = math::random(repeat.decrease_interval.min, repeat.decrease_interval.max);
                    position += duration;
                    for(auto r = 0; r < setting.repeat.time; ++r) {
                        duration -= repeat_decrease_interval;
                        if(setting.duration_in_ticks < position + duration - setting.offset_in_ticks) break;
                        file.addNoteOn(setting.track_id,
                                       position,
                                       0,
                                       note,
                                       100);
                        if(duration < 0) duration = 1;
                        file.addNoteOff(setting.track_id,
                                        position + duration,
                                        0,
                                        note);
                        position += duration;
                    }
                    i = (position - setting.offset_in_ticks) / unit;
                }
            }
        }
    }
};

#endif /* Randomize_h */

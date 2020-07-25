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
#include "Oscillator.h"
#include "Velocity.h"

#include "MidiFile.h"
#include "SimplexNoise.h"
#include "LiveConstants.h"

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
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
        int note_duration = MIDI::SIXTEENTH;
        struct {
            MIDI::Probability probability{1.0f}; // 0.0f - 1.0f
        } random_note;
        struct {
            MIDI::Probability probability{0.0f}; // 0.0f - 1.0f
        } dropout;
        struct {
            MIDI::Probability probability{0.0f}; // 0.0f - 1.0f
//            int time{0};
            MIDI::Range time{0,0};
            int note_duration = MIDI::SIXTEENTH;
            int interval{MIDI::EIGHTH}; // in ticks
            std::vector<int> intervals;
            MIDI::Range decrease_interval;
            MIDI::Range minimum_duration{5, 5};
        } repeat;
        struct {
            std::vector<std::uint8_t> notes; // drunk target
            MIDI::Probability probability{0.0f}; // 0.0f - 1.0f
            MIDI::Range range; // in_ticks drunk min max. can be negative value;
        } drunk;

    };

struct detailed_note{
    MIDI::Choice note;
    struct {
        MIDI::Probability probability{0.0f}; // 0.0f - 1.0f
    } dropout;
    struct {
        MIDI::Probability probability{0.0f}; // 0.0f - 1.0f
//            int time{0};
        MIDI::Range time{0,0};
        int note_duration = MIDI::SIXTEENTH;
        std::vector<int> intervals;
        MIDI::Range decrease_interval;
        MIDI::Range minimum_duration{5, 5};
    } repeat;
    struct {
        MIDI::Probability probability{0.0f}; // 0.0f - 1.0f
        MIDI::Range range; // in_ticks drunk min max. can be negative value;
    } drunk;

};
    struct DetailedSetting : MIDI::Setting {
        int divisor{16};
        int note_duration{MIDI::SIXTEENTH};
        std::vector<detailed_note> notes;
    };
    inline static void make_phrase_with_detail(smf::MidiFile &file,
                                               const DetailedSetting & setting){
        int unit = setting.duration_in_ticks / setting.divisor;
        for(auto &a: setting.notes){
            const auto & repeat = a.repeat;
            const auto & drunk = a.drunk;
            const auto & dropout = a.dropout;
            float normalizer = 1.0f / setting.duration_in_ticks;

            // repeat divisor times
            for(auto i = 0; i < setting.divisor; ++i) {
                //drop out
                if(dropout.probability(i * unit * normalizer) < math::random()) {
                    int position = setting.offset_in_ticks + i * unit;
                    float normalized_position = position * normalizer;
                    int note = a.note();
                    
                    position += drunk.range();
                    if(position < 0) position = 0;
                    normalized_position = position * normalizer;
                    

                    int velocity = 127 ;
                    

                    file.addNoteOn(setting.track_id,
                                   position,
                                   0,
                                   note,
                                   velocity);
                    file.addNoteOff(setting.track_id,
                                    std::min(position + setting.note_duration,
                                             setting.offset_in_ticks + setting.duration_in_ticks),
                                    0,
                                    note);
                    
                    if(0 < repeat.time.random() && math::random() < repeat.probability(normalized_position)
                       && repeat.intervals.size() > 0)
                    {
                        
                        //repeat interval
                        int ik = repeat.intervals.size() * math::random() ;
                        int repeat_duration = repeat.intervals[ik];
                        position += repeat_duration;

                        int repeat_decrease_interval = repeat.decrease_interval();
                        int minimum_duration = repeat.minimum_duration();
                        
                        // repeat
                        for(auto r = 0; r < repeat.time.random(); ++r) {
                            repeat_duration -= repeat_decrease_interval;
                            if(repeat_duration < minimum_duration) repeat_duration = minimum_duration;
                            if(setting.duration_in_ticks < position + repeat_duration - setting.offset_in_ticks) break;
                            file.addNoteOn(setting.track_id,
                                           position,
                                           0,
                                           note,
                                           velocity);
                            file.addNoteOff(setting.track_id,
                                            position + repeat_duration,
                                            0,
                                            note);
                            position += repeat_duration;
                        }
    //                    i = (position - setting.offset_in_ticks) / unit;
                    }
                }
            }
        }
        
        
    }

    inline static void make_phrase(smf::MidiFile &file,
                                   const std::vector<int> &notes,
                                   const PhraseStuttingSetting &setting,
                                   bool notes_in_order = false,
                                   bool repeat_random = true)
    {
        int unit = setting.duration_in_ticks / setting.divisor;
        const auto &repeat = setting.repeat;
        const auto &drunk = setting.drunk;
        float normalizer = 1.0f / setting.duration_in_ticks;
        
        int counter_note = 0;
        // repeat divisor times
        for(auto i = 0; i < setting.divisor; ++i) {
            //drop out
            if(setting.dropout.probability(i * unit * normalizer) < math::random()) {
                int position = setting.offset_in_ticks + i * unit;
                float normalized_position = position * normalizer;
                int note;
                if(notes_in_order){
                    int n = counter_note % notes.size();
                    note = notes[n];
                    counter_note++;
                }
                else{
                    int n = notes.size() * math::random() * setting.random_note.probability(normalized_position);
                    note = notes[n];
                }
                
                bool do_drunk = std::find(drunk.notes.begin(),
                                          drunk.notes.end(),
                                          note) != drunk.notes.end()
                                && drunk.probability(normalized_position);
                
                // drunk
                if(do_drunk) {
                    position += drunk.range();
                    if(position < 0) position = 0;
                    normalized_position = position * normalizer;
                }
                

                int velocity = 127 ;
                

                file.addNoteOn(setting.track_id,
                               position,
                               0,
                               note,
                               velocity);
                file.addNoteOff(setting.track_id,
                                std::min(position + setting.note_duration,
                                         setting.offset_in_ticks + setting.duration_in_ticks),
                                0,
                                note);
                
                if(0 < repeat.time.random() && math::random() < repeat.probability(normalized_position))
                {
                    
                    //repeat interval
                    int repeat_duration;
                    if(repeat.intervals.size() == 0){
                        repeat_duration = repeat.interval;
                    }else{
                        // select interval from intervals randomly
                        int ik = repeat.intervals.size() * math::random() ;
                        repeat_duration = repeat.intervals[ik];
                    }
                    position += repeat_duration;

                    int repeat_decrease_interval = repeat.decrease_interval();
                    int minimum_duration = repeat.minimum_duration();
                    
                    // repeat
                    for(auto r = 0; r < setting.repeat.time.random(); ++r) {
                        repeat_duration -= repeat_decrease_interval;
                        if(repeat_duration < minimum_duration) repeat_duration = minimum_duration;
                        if(setting.duration_in_ticks < position + repeat_duration - setting.offset_in_ticks) break;
                        file.addNoteOn(setting.track_id,
                                       position,
                                       0,
                                       note,
                                       velocity);
                        file.addNoteOff(setting.track_id,
                                        position + repeat_duration,
                                        0,
                                        note);
                        position += repeat_duration;
                    }
//                    i = (position - setting.offset_in_ticks) / unit;
                }
            }
        }
        file.sortTracks();
    } // make_phrase
    // velocity function
    using vel_func = std::function<float(float)>;

    inline static void make_phrase_with_velocity(smf::MidiFile &file,
                                   const std::vector<int> &notes,
                                   const std::unordered_map<int, vel_func> & vel_func_map,
                                   const PhraseStuttingSetting &setting,
                                   bool notes_in_order = false,
                                   bool repeat_random = true)
    {
        int unit = setting.duration_in_ticks / setting.divisor;
        const auto &repeat = setting.repeat;
        const auto &drunk = setting.drunk;
        float normalizer = 1.0f / setting.duration_in_ticks;
        
        int counter_note = 0;
        // repeat divisor times
        for(auto i = 0; i < setting.divisor; ++i) {
            //drop out
            if(setting.dropout.probability(i * unit * normalizer) < math::random()) {
                int position = setting.offset_in_ticks + i * unit;
                float normalized_position = position * normalizer;
                int note;
                if(notes_in_order){
                    int n = counter_note % notes.size();
                    note = notes[n];
                    counter_note++;
                }
                else{
                    int n = notes.size() * math::random() * setting.random_note.probability(normalized_position);
                    note = notes[n];
                }
                
                bool do_drunk = std::find(drunk.notes.begin(),
                                          drunk.notes.end(),
                                          note) != drunk.notes.end()
                                && drunk.probability(normalized_position);
                
                // drunk
                if(do_drunk) {
                    position += drunk.range();
                    if(position < 0) position = 0;
                    normalized_position = position * normalizer;
                }
                
                //velocity
                float t = normalized_position;
                int velocity = 0;
                for(auto &nt : notes){
                    if(vel_func_map.find(nt) != vel_func_map.end()){
                        float tmpv = vel_func_map.at(nt)(t);
                        velocity = 127 * math::map(tmpv, 0, 1., 0.8, 1.0);
                    }
                }
                

                file.addNoteOn(setting.track_id,
                               position,
                               0,
                               note,
                               velocity);
                file.addNoteOff(setting.track_id,
                                std::min(position + setting.note_duration,
                                         setting.offset_in_ticks + setting.duration_in_ticks),
                                0,
                                note);
                
                if(0 < repeat.time.random() && math::random() < repeat.probability(normalized_position))
                {
                    
                    //repeat interval
                    int repeat_duration;
                    if(repeat.intervals.size() == 0){
                        repeat_duration = repeat.interval;
                    }else{
                        // select interval from intervals randomly
                        int ik = repeat.intervals.size() * math::random() ;
                        repeat_duration = repeat.intervals[ik];
                    }
                    position += repeat_duration;

                    int repeat_decrease_interval = repeat.decrease_interval();
                    int minimum_duration = repeat.minimum_duration();
                    
                    // repeat
                    for(auto r = 0; r < setting.repeat.time.random(); ++r) {
                        repeat_duration -= repeat_decrease_interval;
                        if(repeat_duration < minimum_duration) repeat_duration = minimum_duration;
                        if(setting.duration_in_ticks < position + repeat_duration - setting.offset_in_ticks) break;
                        file.addNoteOn(setting.track_id,
                                       position,
                                       0,
                                       note,
                                       velocity);
                        file.addNoteOff(setting.track_id,
                                        position + repeat_duration,
                                        0,
                                        note);
                        position += repeat_duration;
                    }
//                    i = (position - setting.offset_in_ticks) / unit;
                }
            }
        }
        file.sortTracks();
    } // make_phrase
};

#endif /* Randomize_h */

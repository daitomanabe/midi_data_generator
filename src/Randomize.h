//
//  Randomize.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/10.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef Randomize_h
#define Randomize_h

#include "MathConstants.h"
#include "MathUtils.h"

#include "MidiFile.h"

#include <set>
#include <map>
#include <vector>
#include <iostream>

#include <cstddef>
#include <cstdint>

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

smf::MidiFile randomize_note(const smf::MidiFile &original,
                             int track_id)
{
    smf::MidiFile file;
    int new_track = file.addTrack();
    
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
            new_event.track = new_track;
            file.addEvent(new_event);
        }
    }
    
    std::vector<std::uint8_t> pitches;
    pitches.reserve(pitch_set.size());
    std::copy(pitch_set.cbegin(), pitch_set.cend(), std::back_inserter(pitches));
    
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
                file.addNoteOn(new_track,
                               note.tick,
                               note.channel,
                               pitch,
                               note.velocity);
                if(i + i < notes.size() && !notes[i + 1].is_on) {
                    file.addNoteOff(new_track,
                                    notes[i + 1].tick,
                                    notes[i + 1].channel,
                                    pitch);
                }
            }
        }
    }
    
    file.sortTracks();
    return file;
}

#endif /* Randomize_h */

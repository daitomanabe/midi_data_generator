//
//  MidiFileUtils.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/17.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef MidiFileUtils_h
#define MidiFileUtils_h

#include "MidiConstants.h"

#include "MidiFile.h"

#include <functional>
#include <vector>
#include <map>

namespace MIDI {
    namespace detail {
        struct pitch_id {
            pitch_id() = default;
            pitch_id(std::uint8_t channel, std::uint8_t pitch, int track_id = 0)
            : channel{channel}
            , pitch{pitch}
            , track_id{track_id}
            {};
            
            std::uint8_t channel;
            std::uint8_t pitch;
            int track_id;
            
            inline std::uint16_t hash() const
            { return track_id * 16384 + channel * 128 + pitch; };
            
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
    
    struct StructuredNotes {
        struct Note {
            Note() {};
            Note(const Note &) = default;
            Note(Note &&) = default;
            Note &operator=(const Note &) = default;
            Note &operator=(Note &&) = default;

            struct {
                int from;
                int to;
            } range;
            int pitch;
            int velocity;
            
            int channel;
            int track_id;
        };
        
        StructuredNotes() = default;
        StructuredNotes(const StructuredNotes &) = default;
        StructuredNotes(StructuredNotes &&) = default;
        
        StructuredNotes(const smf::MidiFile &file)
        : tpq{file.getTPQ()}
        {
            for(int track_id = 0; track_id < file.getNumTracks(); ++track_id) {
                std::map<detail::pitch_id, std::vector<detail::midi_note>> separated_notes;
                const auto &event_list = file[track_id];
                
                for(auto i = 0; i < event_list.size(); ++i) {
                    const auto &event = event_list[i];
                    if(event.isNote()) {
                        auto &notes = separated_notes[detail::pitch_id(event.getChannel(), event.getKeyNumber(), track_id)];
                        notes.emplace_back();
                        detail::midi_note &note = notes.back();
                        note.track_id = event.track;
                        note.channel = event.getChannel();
                        note.pitch = event.getKeyNumber();
                        note.velocity = event.getVelocity();
                        note.is_on = event.isNoteOn();
                        note.tick = event.tick;
                    }
                }
                
                for(auto &pair : separated_notes) {
                    auto &note_set = pair.second;
                    std::sort(note_set.begin(),
                              note_set.end(),
                              [](const detail::midi_note &x,
                                 const detail::midi_note &y)
                              { return x.tick < y.tick; });
                    for(auto i = 0; i < note_set.size(); ++i) {
                        const auto &note = note_set[i];
                        if(note.is_on) {
                            auto new_note = Note{};
                            new_note.channel = note.channel;
                            new_note.pitch = note.pitch;
                            new_note.range.from = note.tick;
                            new_note.velocity = note.velocity;
                            new_note.track_id = note.track_id;
                            if(i + 1 < note_set.size()) {
                                new_note.range.to = note_set[i + 1].tick;
                                if(!note_set[i + 1].is_on) i++;
                            } else {
                                new_note.range.to = new_note.range.from + 10;
                            }
                            notes[track_id].push_back(new_note);
                        }
                    }
                }
            }
        }
        
        std::vector<Note> findPlayingNotesIn(int track_id, int from, int to) const {
            std::vector<Note> results;
            if(notes.find(track_id) == notes.end()) return results;
            const auto &track_notes = notes.at(track_id);
            std::copy_if(track_notes.begin(),
                         track_notes.end(),
                         std::back_inserter(results),
                         [from, to](const Note &note)
                         { return from <= note.range.to && note.range.from <= to; });
            return results;
        }
        
        std::vector<Note> findPlayingNotesAt(int track_id, int tick) const {
            std::vector<Note> results;
            if(notes.find(track_id) == notes.end()) return results;
            const auto &track_notes = notes.at(track_id);
            std::copy_if(track_notes.begin(),
                         track_notes.end(),
                         std::back_inserter(results),
                         [tick](const Note &note)
                         { return note.range.from <= tick && tick < note.range.to; });
            return results;
        }

        std::vector<Note> findStartNotesIn(int track_id, int from, int to) const {
            std::vector<Note> results;
            if(notes.find(track_id) == notes.end()) return results;
            const auto &track_notes = notes.at(track_id);
            std::copy_if(track_notes.begin(),
                         track_notes.end(),
                         std::back_inserter(results),
                         [from, to](const Note &note)
                         { return from <= note.range.from && note.range.from < to; });
            return results;
        }

        std::vector<Note> findStartNotesAt(int track_id, int tick) const {
            std::vector<Note> results;
            if(notes.find(track_id) == notes.end()) return results;
            const auto &track_notes = notes.at(track_id);
            std::copy_if(track_notes.begin(),
                         track_notes.end(),
                         std::back_inserter(results),
                         [tick](const Note &note)
                         { return note.range.from == tick; });
            return results;
        }

        std::map<int, std::vector<Note>> notes; // track_id, notes
        int tpq;
    };

    void mergeTracks(smf::MidiFile &src_file,
                     int src_track_id,
                     smf::MidiFile &dst_file,
                     int dst_track_id)
    {
        const auto &src_events = src_file[src_track_id];
        for(auto i = 0; i < src_events.size(); ++i) {
            auto e = src_events[i];
            dst_file[dst_track_id].append(e);
        }
    }
                    
    void convertTPQ(smf::MidiFile &file,
                    int tpq)
    {
        float factor = (float)tpq / file.getTPQ();
        for(auto track_id = 0; track_id < file.getTrackCount(); ++track_id) {
            auto &events = file[track_id];
            for(auto i = 0; i < events.size(); ++i) {
                events[i].tick *= factor;
            }
        }
        file.setTPQ(tpq);
        file.sortTracks();
    }
};

#endif /* MidiFileUtils_h */

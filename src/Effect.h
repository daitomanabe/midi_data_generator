//
//  Effect.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/17.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef Effect_h
#define Effect_h

#include "MidiConstants.h"
#include "MidiSetting.h"
#include "MidiFileUtils.h"

#include "MathConstants.h"
#include "MathUtils.h"

#include "MidiFile.h"

#include <iostream>

namespace MIDI {
    namespace Effect {
        struct StutterParameter {
            int div_ticks;
            struct {
                MIDI::Probability probability{0.0f}; // 0.0f - 1.0f
                int time{0};
                MIDI::Range interval{MIDI::SIXTEENTH, MIDI::EIGHTH};
                MIDI::Range decrease_interval{0, 0};
            } repeat;
        };
        inline static void stutter(smf::MidiFile &file,
                                   int track_id,
                                   const StutterParameter &p)
        {
            StructuredNotes tracked_notes{file};
            if(tracked_notes.notes.find(track_id) == tracked_notes.notes.end()) {
                std::cerr << "track_id " << track_id << " is not found on source file" << std::endl;
                return;
            }
            file[track_id].clear();
            auto max_note = std::max_element(tracked_notes.notes.at(track_id).begin(),
                                             tracked_notes.notes.at(track_id).end(),
                                             [](MIDI::StructuredNotes::Note x,
                                                MIDI::StructuredNotes::Note y)
                                             { return x.range.to < y.range.to; });
            auto num_unit = max_note->range.to / p.div_ticks + 1;
            float max_tick = num_unit * p.div_ticks;
            std::vector<StructuredNotes::Note> playing_notes;
            for(auto i = 0; i < num_unit; i++) {
                int from = i * p.div_ticks,
                    to = (i + 1) * p.div_ticks;
                if(math::random() < p.repeat.probability(i / max_tick)) {
                    auto &&notes = tracked_notes
                        .findStartNotesAt(track_id, from);
                    auto stutting_notes = playing_notes;
                    std::copy(notes.begin(),
                              notes.end(),
                              std::back_inserter(stutting_notes));
                    int current_tick = from;
                    int interval = p.repeat.interval();
                    int decrease_interval = p.repeat.decrease_interval();
                    while(current_tick < to) {
                        int f = current_tick, t = current_tick + interval;
                        if(to < t) t = to;
                        for(const auto &note : stutting_notes) {
                            file.addNoteOn(track_id,
                                           f,
                                           0,
                                           note.pitch,
                                           note.velocity);
                            file.addNoteOff(track_id,
                                            t,
                                            0,
                                            note.pitch);
                        }
                        current_tick += interval;
                        interval -= decrease_interval;
                        if(interval < 5) interval = 5;
                    }
                    for(const auto &note : stutting_notes) {
                        if(to < note.range.to) {
                            file.addNoteOn(track_id,
                                           to,
                                           0,
                                           note.pitch,
                                           note.velocity);
                        }
                    }
                    std::copy(notes.begin(),
                              notes.end(),
                              std::back_inserter(playing_notes));
                    auto it = std::remove_if(playing_notes.begin(),
                                             playing_notes.end(),
                                             [to] (const StructuredNotes::Note &note)
                                             { return note.range.to <= to; });
                    playing_notes.erase(it, playing_notes.end());
                } else {
                    auto &&notes = tracked_notes
                        .findStartNotesIn(track_id, from, to);
                    for(const auto &note : playing_notes) {
                        if(note.range.to <= to) {
                            file.addNoteOff(track_id,
                                            note.range.to,
                                            note.channel,
                                            note.pitch);
                        }
                    }
                    for(const auto &note : notes) {
                        file.addNoteOn(track_id,
                                       note.range.from,
                                       note.channel,
                                       note.pitch,
                                       note.velocity);
                        if(note.range.to <= to) {
                            file.addNoteOff(track_id,
                                            note.range.to,
                                            note.channel,
                                            note.pitch);
                        } else {
                            playing_notes.push_back(note);
                        }
                    }
                    
                    auto it = std::remove_if(playing_notes.begin(),
                                             playing_notes.end(),
                                             [to] (const StructuredNotes::Note &note)
                                             { return note.range.to <= to; });
                    notes.erase(playing_notes.end(), it);
                }
            }
            file.sortTrack(track_id);
        } // stutter
    };
};

#endif /* Effects_h */

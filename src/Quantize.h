//
//  Quantize.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/11.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef Quantize_h
#define Quantize_h

#include "MathConstants.h"
#include "MathUtils.h"

#include "MidiConstants.h"
#include "MidiSetting.h"

#include "MidiFile.h"

#include <set>
#include <map>
#include <vector>
#include <iostream>

#include <cstddef>
#include <cstdint>

namespace quantize {
    /**
    \function by_per_ticks
    \param file base file
    \param curve  required: curve: [0, 1) -> [0, 1) && x < y --> curve(x) < curve(y)
    \param unit_ticks default: 480 == ONEMEASURE
    \param track_id default: -1, if negative number is given, then process for all tracks
    */
    static void per_ticks(smf::MidiFile &file,
                   std::function<float(float)> curve,
                   std::size_t unit_ticks = MIDI::ONEMEASURE,
                   int track_id = -1)
    {
        if(track_id < 0) {
            for(auto i = 0; i < file.getNumTracks(); ++i) {
                per_ticks(file, curve, unit_ticks, i);
            }
            return;
        } else if(file.getNumTracks() < track_id) {
            std::cerr << "given track_id is out of bounds: track_id must be less than " << file.getNumTracks() << " but " << track_id << " is given." << std::endl;
        }
    
        auto &event_list = file[track_id];
        for(auto i = 0; i < event_list.size(); ++i) {
            auto &event = event_list[i];
            int tick_num = event.tick / unit_ticks;
            int tick_offset = event.tick % unit_ticks;
            event.tick = tick_num * unit_ticks + curve(tick_offset / (float)unit_ticks) * unit_ticks;
        }
        file.sortTrack(track_id);
    }
    
    static void per_ticks(smf::MidiFile &file,
                   std::function<float(float)> curve,
                   MIDI::Setting setting = {MIDI::AllTrack, MIDI::ONEMEASURE, 0})
    {
        per_ticks(file, curve, setting.duration_in_ticks, setting.track_id);
    }
};

#endif /* Quantize_h */

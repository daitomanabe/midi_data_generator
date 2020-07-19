//
//  Control.h
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/03/24.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef Control_h
#define Control_h

#include "MidiFile.h"
#include "MidiSetting.h"
#include "MidiFileUtils.h"
#include "MidiEvent.h"
#include <iostream>
#include <functional>
#include <initializer_list>
#include <vector>
#include <string>
#include <istream>
#include <fstream>
#include "Easing.h"

typedef std::function<float (float)> EaseFn;

namespace control{



inline void add_ctrl_event(smf::MidiFile& midifile, int track_id,
                           int channel,
                           int val,
                           int duration_in_ticks){
    int ctrl_change = 0xB0;
    std::vector<smf::uchar> midievent;
    midievent.resize(3);
    midievent[0] = ctrl_change;
    midievent[1] = channel;
    midievent[2] = val;
    midifile.addEvent(track_id, duration_in_ticks, midievent);
}

inline void add_ctrl_events(smf::MidiFile& midifile,
                            int track_id,
                            int channel,
                            float min_val,
                            float max_val,
                            const EaseFn & fn,
                            int duration_in_ticks = MIDI::ONEMEASURE,
                            float resolution_in_ticks = 5
                            )
{
    int div = duration_in_ticks / resolution_in_ticks;
    int action_time = 0;
    
    for(int i=0; i<div; i++){
        float current_pos_norm = i / static_cast<float>(div);
        smf::uchar val = static_cast<smf::uchar>(fn(current_pos_norm) * 128);
        add_ctrl_event(midifile, track_id, channel, val, action_time);
        action_time += resolution_in_ticks;
    }
}

//
//void addPitchBendTest(smf::MidiFile& midifile,
//int track_id,
//int ctrl_ch = 1,
//int velocity = 127,
//const EaseFn &ease_fn = &choreograph::easeNone,
//int div = 8,
//int duration_in_ticks = MIDI::ONEMEASURE)
//{
//
//}

};

#endif /* Control_h */

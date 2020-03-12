//
//  MidiFileSetting.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/13.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef MidiSetting_h
#define MidiSetting_h

#include "MidiConstants.h"

namespace MIDI {
    static constexpr int AllTrack = -1;
    struct Setting {
        int track_id{0};
        int duration_in_ticks{ONEMEASURE};
        int offset_in_ticks{0};
    };
};

#endif /* MidiFileSetting_h */

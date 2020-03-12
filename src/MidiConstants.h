//
//  MidiConstants.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/12.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef MidiConstants_h
#define MidiConstants_h

namespace MIDI {
    static constexpr int NOTE_OFF = 0x80;
    static constexpr int NOTE_ON = 0x90;
    static constexpr int POLYPHONIC_PRESSURE = 0xA0;
    static constexpr int CONTROL_CHANGE = 0xB0;
    static constexpr int PROGRAM_CHANGE = 0xC0;
    static constexpr int CHANNEL_PRESSURE = 0xD0;
    static constexpr int PITCHBEND = 0xE0;

    static constexpr int HIGH_HAT    = 59;
    static constexpr int SNARE       = 38;
    static constexpr int BASS_DRUM   = 36;
     
    static constexpr int ONEMEASURE   = 480;
    static constexpr int QUARTER      = 120; /* ticks per quarter note */
    static constexpr int EIGHTH       = 60;
    static constexpr int SIXTEENTH    = 30;
    static constexpr int THIRTYSECOND = 15;
};

#endif /* Constants_h */

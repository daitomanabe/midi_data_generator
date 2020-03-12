//
//  MidiConstants.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/12.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef MidiConstants_h
#define MidiConstants_h

#include <cstddef>
#include <cstdint>

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
    
    static constexpr std::uint8_t C_m2 = 0;
    static constexpr std::uint8_t Db_m2 = 1;
    static constexpr std::uint8_t Cs_m2 = 1;
    static constexpr std::uint8_t D_m2 = 2;
    static constexpr std::uint8_t Eb_m2 = 3;
    static constexpr std::uint8_t Ds_m2 = 3;
    static constexpr std::uint8_t E_m2 = 4;
    static constexpr std::uint8_t F_m2 = 5;
    static constexpr std::uint8_t Gb_m2 = 6;
    static constexpr std::uint8_t Fs_m2 = 6;
    static constexpr std::uint8_t G_m2 = 7;
    static constexpr std::uint8_t Ab_m2 = 8;
    static constexpr std::uint8_t Gs_m2 = 8;
    static constexpr std::uint8_t A_m2 = 9;
    static constexpr std::uint8_t Bb_m2 = 10;
    static constexpr std::uint8_t As_m2 = 10;
    static constexpr std::uint8_t B_m2 = 11;
    static constexpr std::uint8_t C_m1 = 12;
    static constexpr std::uint8_t Db_m1 = 13;
    static constexpr std::uint8_t Cs_m1 = 13;
    static constexpr std::uint8_t D_m1 = 14;
    static constexpr std::uint8_t Eb_m1 = 15;
    static constexpr std::uint8_t Ds_m1 = 15;
    static constexpr std::uint8_t E_m1 = 16;
    static constexpr std::uint8_t F_m1 = 17;
    static constexpr std::uint8_t Gb_m1 = 18;
    static constexpr std::uint8_t Fs_m1 = 18;
    static constexpr std::uint8_t G_m1 = 19;
    static constexpr std::uint8_t Ab_m1 = 20;
    static constexpr std::uint8_t Gs_m1 = 20;
    static constexpr std::uint8_t A_m1 = 21;
    static constexpr std::uint8_t Bb_m1 = 22;
    static constexpr std::uint8_t As_m1 = 22;
    static constexpr std::uint8_t B_m1 = 23;
    static constexpr std::uint8_t C_0 = 24;
    static constexpr std::uint8_t Db_0 = 25;
    static constexpr std::uint8_t Cs_0 = 25;
    static constexpr std::uint8_t D_0 = 26;
    static constexpr std::uint8_t Eb_0 = 27;
    static constexpr std::uint8_t Ds_0 = 27;
    static constexpr std::uint8_t E_0 = 28;
    static constexpr std::uint8_t F_0 = 29;
    static constexpr std::uint8_t Gb_0 = 30;
    static constexpr std::uint8_t Fs_0 = 30;
    static constexpr std::uint8_t G_0 = 31;
    static constexpr std::uint8_t Ab_0 = 32;
    static constexpr std::uint8_t Gs_0 = 32;
    static constexpr std::uint8_t A_0 = 33;
    static constexpr std::uint8_t Bb_0 = 34;
    static constexpr std::uint8_t As_0 = 34;
    static constexpr std::uint8_t B_0 = 35;
    static constexpr std::uint8_t C_1 = 36;
    static constexpr std::uint8_t Db_1 = 37;
    static constexpr std::uint8_t Cs_1 = 37;
    static constexpr std::uint8_t D_1 = 38;
    static constexpr std::uint8_t Eb_1 = 39;
    static constexpr std::uint8_t Ds_1 = 39;
    static constexpr std::uint8_t E_1 = 40;
    static constexpr std::uint8_t F_1 = 41;
    static constexpr std::uint8_t Gb_1 = 42;
    static constexpr std::uint8_t Fs_1 = 42;
    static constexpr std::uint8_t G_1 = 43;
    static constexpr std::uint8_t Ab_1 = 44;
    static constexpr std::uint8_t Gs_1 = 44;
    static constexpr std::uint8_t A_1 = 45;
    static constexpr std::uint8_t Bb_1 = 46;
    static constexpr std::uint8_t As_1 = 46;
    static constexpr std::uint8_t B_1 = 47;
    static constexpr std::uint8_t C_2 = 48;
    static constexpr std::uint8_t Db_2 = 49;
    static constexpr std::uint8_t Cs_2 = 49;
    static constexpr std::uint8_t D_2 = 50;
    static constexpr std::uint8_t Eb_2 = 51;
    static constexpr std::uint8_t Ds_2 = 51;
    static constexpr std::uint8_t E_2 = 52;
    static constexpr std::uint8_t F_2 = 53;
    static constexpr std::uint8_t Gb_2 = 54;
    static constexpr std::uint8_t Fs_2 = 54;
    static constexpr std::uint8_t G_2 = 55;
    static constexpr std::uint8_t Ab_2 = 56;
    static constexpr std::uint8_t Gs_2 = 56;
    static constexpr std::uint8_t A_2 = 57;
    static constexpr std::uint8_t Bb_2 = 58;
    static constexpr std::uint8_t As_2 = 58;
    static constexpr std::uint8_t B_2 = 59;
    static constexpr std::uint8_t C_3 = 60;
    static constexpr std::uint8_t Db_3 = 61;
    static constexpr std::uint8_t Cs_3 = 61;
    static constexpr std::uint8_t D_3 = 62;
    static constexpr std::uint8_t Eb_3 = 63;
    static constexpr std::uint8_t Ds_3 = 63;
    static constexpr std::uint8_t E_3 = 64;
    static constexpr std::uint8_t F_3 = 65;
    static constexpr std::uint8_t Gb_3 = 66;
    static constexpr std::uint8_t Fs_3 = 66;
    static constexpr std::uint8_t G_3 = 67;
    static constexpr std::uint8_t Ab_3 = 68;
    static constexpr std::uint8_t Gs_3 = 68;
    static constexpr std::uint8_t A_3 = 69;
    static constexpr std::uint8_t Bb_3 = 70;
    static constexpr std::uint8_t As_3 = 70;
    static constexpr std::uint8_t B_3 = 71;
    static constexpr std::uint8_t C_4 = 72;
    static constexpr std::uint8_t Db_4 = 73;
    static constexpr std::uint8_t Cs_4 = 73;
    static constexpr std::uint8_t D_4 = 74;
    static constexpr std::uint8_t Eb_4 = 75;
    static constexpr std::uint8_t Ds_4 = 75;
    static constexpr std::uint8_t E_4 = 76;
    static constexpr std::uint8_t F_4 = 77;
    static constexpr std::uint8_t Gb_4 = 78;
    static constexpr std::uint8_t Fs_4 = 78;
    static constexpr std::uint8_t G_4 = 79;
    static constexpr std::uint8_t Ab_4 = 80;
    static constexpr std::uint8_t Gs_4 = 80;
    static constexpr std::uint8_t A_4 = 81;
    static constexpr std::uint8_t Bb_4 = 82;
    static constexpr std::uint8_t As_4 = 82;
    static constexpr std::uint8_t B_4 = 83;
    static constexpr std::uint8_t C_5 = 84;
    static constexpr std::uint8_t Db_5 = 85;
    static constexpr std::uint8_t Cs_5 = 85;
    static constexpr std::uint8_t D_5 = 86;
    static constexpr std::uint8_t Eb_5 = 87;
    static constexpr std::uint8_t Ds_5 = 87;
    static constexpr std::uint8_t E_5 = 88;
    static constexpr std::uint8_t F_5 = 89;
    static constexpr std::uint8_t Gb_5 = 90;
    static constexpr std::uint8_t Fs_5 = 90;
    static constexpr std::uint8_t G_5 = 91;
    static constexpr std::uint8_t Ab_5 = 92;
    static constexpr std::uint8_t Gs_5 = 92;
    static constexpr std::uint8_t A_5 = 93;
    static constexpr std::uint8_t Bb_5 = 94;
    static constexpr std::uint8_t As_5 = 94;
    static constexpr std::uint8_t B_5 = 95;
    static constexpr std::uint8_t C_6 = 96;
    static constexpr std::uint8_t Db_6 = 97;
    static constexpr std::uint8_t Cs_6 = 97;
    static constexpr std::uint8_t D_6 = 98;
    static constexpr std::uint8_t Eb_6 = 99;
    static constexpr std::uint8_t Ds_6 = 99;
    static constexpr std::uint8_t E_6 = 100;
    static constexpr std::uint8_t F_6 = 101;
    static constexpr std::uint8_t Gb_6 = 102;
    static constexpr std::uint8_t Fs_6 = 102;
    static constexpr std::uint8_t G_6 = 103;
    static constexpr std::uint8_t Ab_6 = 104;
    static constexpr std::uint8_t Gs_6 = 104;
    static constexpr std::uint8_t A_6 = 105;
    static constexpr std::uint8_t Bb_6 = 106;
    static constexpr std::uint8_t As_6 = 106;
    static constexpr std::uint8_t B_6 = 107;
    static constexpr std::uint8_t C_7 = 108;
    static constexpr std::uint8_t Db_7 = 109;
    static constexpr std::uint8_t Cs_7 = 109;
    static constexpr std::uint8_t D_7 = 110;
    static constexpr std::uint8_t Eb_7 = 111;
    static constexpr std::uint8_t Ds_7 = 111;
    static constexpr std::uint8_t E_7 = 112;
    static constexpr std::uint8_t F_7 = 113;
    static constexpr std::uint8_t Gb_7 = 114;
    static constexpr std::uint8_t Fs_7 = 114;
    static constexpr std::uint8_t G_7 = 115;
    static constexpr std::uint8_t Ab_7 = 116;
    static constexpr std::uint8_t Gs_7 = 116;
    static constexpr std::uint8_t A_7 = 117;
    static constexpr std::uint8_t Bb_7 = 118;
    static constexpr std::uint8_t As_7 = 118;
    static constexpr std::uint8_t B_7 = 119;
    static constexpr std::uint8_t C_8 = 120;
    static constexpr std::uint8_t Db_8 = 121;
    static constexpr std::uint8_t Cs_8 = 121;
    static constexpr std::uint8_t D_8 = 122;
    static constexpr std::uint8_t Eb_8 = 123;
    static constexpr std::uint8_t Ds_8 = 123;
    static constexpr std::uint8_t E_8 = 124;
    static constexpr std::uint8_t F_8 = 125;
    static constexpr std::uint8_t Gb_8 = 126;
    static constexpr std::uint8_t Fs_8 = 126;
    static constexpr std::uint8_t G_8 = 127;
};

#endif /* Constants_h */

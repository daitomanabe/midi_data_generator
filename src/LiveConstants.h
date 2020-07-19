//
//  LiveConstants.h
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/03/21.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef LiveConstants_h
#define LiveConstants_h

#include <cstddef>
#include <cstdint>

namespace LIVE {
    static constexpr std::uint8_t MUTE = 0;
    static constexpr std::uint8_t KICK = 36;
    static constexpr std::uint8_t RIM = 37;
    static constexpr std::uint8_t SNARE = 38;
    static constexpr std::uint8_t CLAP = 39;
    static constexpr std::uint8_t CLAVE = 40;
    static constexpr std::uint8_t TOMLOW = 41;
    static constexpr std::uint8_t HATCLOSE = 42;
    static constexpr std::uint8_t TOMMID = 43;
    static constexpr std::uint8_t MARACAS = 44;
    static constexpr std::uint8_t TOMHIGH = 45;
    static constexpr std::uint8_t HATOPEN = 46;
    static constexpr std::uint8_t CONGALOW = 47;
    static constexpr std::uint8_t CONGAMID = 48;
    static constexpr std::uint8_t CYMBAL = 49;
    static constexpr std::uint8_t CONGAHIGH = 50;
    static constexpr std::uint8_t COWBELL = 51;
};
#endif /* LiveConstants_h */

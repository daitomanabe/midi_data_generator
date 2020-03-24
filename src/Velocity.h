//
//  Velocity.h
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/03/24.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//
// change velocity of midi file
#ifndef Velocity_h
#define Velocity_h

#include "MidiConstants.h"
#include "MidiSetting.h"
#include "MathConstants.h"
#include "MathUtils.h"

#include "MidiFile.h"
#include "SimplexNoise.h"

namespace velocity {

    struct VelocityEnvelopeSetting : MIDI::Setting {
        std::function<float(float)> fn; // perlin, oscillator, etc
        MIDI::Range range;
        // add random value to velocity curve
        struct {
            MIDI::Probability probability{0.0f};
            MIDI::Range range;
        } drunk;
    };
    void make_velocity_envelope(smf::MidiFile &file,
                             const std::vector<int> &notes,
                            const VelocityEnvelopeSetting &ves){
        
    }

};
#endif /* Velocity_h */

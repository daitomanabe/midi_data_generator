//
//  Oscillator.h
//  midi_data_generator
//
//  Created by 2bit on 2020/03/12.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#ifndef Oscillator_h
#define Oscillator_h

#include "MathConstants.h"
#include "MathUtils.h"

#include <cmath>

//pitch bend , ctrl 用
//tri,sin,cos,noise,
//周波数はtick単位

namespace oscillator{
    inline float cycle(float t, float freq, float phase){
        // add 0.5 to time for starting from zero value.
        return (std::cos(TWO_PI * (t + 0.5) * freq + phase) + 1.) * 0.5;
    }
    //train~
    //WIP
    inline float train(float t, float freq, float duty_ratio, float phase){
        return (std::fmod(t, 1.0f/ freq) < duty_ratio / freq) ? 0.f : 1.0f;
    }
    // tri~
    inline float tri(float t, float freq, float duty_cycle = 0.5f) {
        return std::fabs(std::fmod(std::fabs(t * freq), 1) - 0.5f) * 2.0f;
    }
    // phasor~
    inline float phasor(float t, float freq) {
        float v = t * freq;
        return (v < 0.0) ? std::fmod(1.0 - std::fmod(-v, 1.0), 1.0) : std::fmod(v, 1.0);
    }
};

using OscFn = std::function<float (float, float, float)>;

#endif /* Oscillator_h */

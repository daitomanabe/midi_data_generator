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

#include <functional>

namespace MIDI {
    static constexpr int AllTrack = -1;
    struct Setting {
        Setting() = default;
        Setting(int track_id, int duration_in_ticks, int offset_in_ticks)
        : track_id{track_id}
        , duration_in_ticks{duration_in_ticks}
        , offset_in_ticks{offset_in_ticks}
        {};
        
        int track_id{0};
        int duration_in_ticks{ONEMEASURE};
        int offset_in_ticks{0};
    };
    
    struct Probability {
        Probability()
        : Probability{0.0f}
        {};
        
        Probability(float v)
        : value{[v](float){ return v; }}
        {};
        
        Probability(std::function<float(float)> v)
        : value{v}
        {};

        Probability(const Probability &) = default;
        Probability(Probability &&) = default;

        template <typename type>
        Probability operator=(type &&v)
        {
            value = Probability{std::forward<type>(v)}.value;
            return *this;
        }
        
        operator float() const
        { return value(0.0f); };
        
        float operator()(float t) const
        { return value(t); };
        
        std::function<float(float)> value;
    };
};

#endif /* MidiFileSetting_h */

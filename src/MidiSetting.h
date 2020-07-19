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
#include "MathUtils.h"
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

        Probability(double v)
        : value{[v](double){ return v; }}
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
    
    struct Range {
        Range() = default;
        Range(const Range &) = default;
        Range(int min, int max)
        : min{min}
        , max{max}
        {};
        
        Range &operator=(const Range &) = default;
        
        int min;
        int max;
        int random() const
        { return math::random(min, max); };
        inline int operator()() const
        { return random(); };
        
        Range &operator=(int x) {
            min = max = x;
            return *this;
        };
        
        operator int() const
        { return max; };
    };
    
    struct TickRange {
        TickRange() = default;
        TickRange(const TickRange &) = default;
        TickRange(int from, int to)
        : from{from}
        , to{to}
        {};
        
        TickRange &operator=(const TickRange &) = default;
        
        int from;
        int to;
        int random() const
        { return math::random(from, to); };
        inline int operator()() const
        { return random(); };
        
        TickRange &operator=(int x) {
            from = to = x;
            return *this;
        };
        
        operator int() const
        { return from; };
    };
struct Choice {
    std::vector<int> choices;
    Choice() = default;
    Choice(const Choice &) = default;
    Choice(const std::vector<int> cs)
    : choices{cs}
    {};
    
    Choice &operator=(const Choice &) = default;
    
//    int min;
//    int max;
    int random() const
    {
        if(choices.size()==0){
            return -1;
        }
        else{
            int k = math::random(0, choices.size()-1);
            return choices[k];
        }
    }
    inline int operator()() const
    { return random(); };
    
    Choice &operator=(int x) {
        choices.clear();
        choices.emplace_back(x);
        return *this;
    };
    void push_back(int a){
        choices.emplace_back(a);
    }
    void operator <<(int k){
        choices.emplace_back(k);
    }
    operator int() const
    {
        if(choices.size()==0){
            return -1;
        }
        else{
            return choices[0];
        }
    };
};

};

#endif /* MidiFileSetting_h */

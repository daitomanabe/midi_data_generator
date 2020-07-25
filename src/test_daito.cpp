//
//  test_daito.cpp
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/03/17.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#include "test_daito.hpp"

#include "Randomize.h"
//#include "Quantize.h"
#include "Effect.h"
#include "Sequencer.h"
#include "Tidaloid.h"
#include "LiveConstants.h"
#include "Control.h"
#include "Oscillator.h"
//#include <unordered_map>


using namespace std;

namespace test_daito {

    void test_phrase(std::string exportDir, int cur, int total) {
        float step = cur / static_cast<float>(total);
         math::set_seed_fast();
         smf::MidiFile file;
         randomize::PhraseStuttingSetting setting;

        int mul = 32;
        int dur = MIDI::ONEMEASURE * mul;
        int div = 8 * mul;
         setting.duration_in_ticks = dur;

         int tid = file.addTrack();
         setting.track_id = tid;
         setting.dropout.probability = math::map(step, 0, 1, 0.1f, 0.5f);
         setting.divisor = div;
         auto &repeat = setting.repeat;
         repeat.probability = math::map(step, 0, 1, 0.1, 0.8f);
         repeat.time = math::map(step, 0, 1, 1, 20);
         repeat.interval = MIDI::EIGHTH;
         repeat.decrease_interval.min = math::map(step, 0, 1, 2, 5);
         repeat.decrease_interval.max = math::map(step, 0, 1, 4, 20);
         
         randomize::make_phrase(file,
                                {LIVE::KICK},
                                setting);
        EaseFn fnk = [&](float t) { return oscillator::cycle(t, cur * 2, 0.); };
        EaseFn fnk2 = [&](float t) {
            float pval  =
            oscillator::train(t, cur * 0.1f, 0.1f, 0.)
            *
            oscillator::cycle(t, cur * 1.33333, 0.);
            return pval ;
        };
        EaseFn fnk3 = [&](float t) {
            float pval  =
            oscillator::train(t, cur * 0.25f, 0.1f, 0.)
            *
            oscillator::tri(t, cur * 1.33333, 0.3333f);
            return pval ;
        };
        EaseFn fnk4 = [&](float t) {
            float pval  =
            oscillator::phasor(t, cur * 0.125f)
            *
            oscillator::tri(t, cur * 1.33333f, 0.3333f)
            *
            oscillator::cycle(t, cur * 2., 0.5f);
            ;
            return pval ;
        };



         //
         setting.divisor = div * 2;
         setting.track_id = file.addTrack();
         setting.dropout.probability = math::map(step, 0, 1, 0.5f, 0.8f);
         repeat.time = math::map(step, 0, 1, 1, 10);
         repeat.interval = MIDI::THIRTYSECOND;
         repeat.decrease_interval.min = math::map(step, 0, 1, 1, 10);
         repeat.decrease_interval.max = math::map(step, 0, 1, 1, 100);
         repeat.probability = [](float t) { return math::random(1.0) * t; };
         
         auto &drunk = setting.drunk;
         drunk.probability =  math::map(step, 0, 1, 0, 0.8f);
         drunk.range.min = 0;
         drunk.range.max =  math::map(step, 0, 1, 1, MIDI::SIXTEENTH);
         drunk.notes = {LIVE::SNARE,LIVE::RIM,LIVE::CLAP};

         randomize::make_phrase(file,
                                {LIVE::SNARE,LIVE::RIM,LIVE::CLAP,LIVE::HATCLOSE},
                                setting);
        
        MIDI::Effect::StutterParameter sp;
        sp.div_ticks = 16;
        sp.repeat.probability = 0.3;
        sp.repeat.interval = {MIDI::THIRTYSECOND, MIDI::EIGHTH};
        sp.repeat.decrease_interval = {1, 30};
        sp.repeat.minimum_interval = {1,5};
        MIDI::Effect::stutter(file, MIDI::AllTrack, sp);
        int track_id = file.addTrack();
        randomize::fill_by_perlin(file, cur, {LIVE::TOMLOW, LIVE::TOMMID, LIVE::TOMHIGH},
                       0.5,
                       div,
                       dur);
        track_id = file.addTrack();
        randomize::arpeggiate_by_perlin(file,cur,
                                        {MIDI::C_4, MIDI::D_4, MIDI::Eb_4, MIDI::F_4, MIDI::G_4, MIDI::A_5 },
                                        div,
                                        dur);
        
        control::add_ctrl_events(file,
                                tid,
                                1,
                                0,
                                127,
                                fnk);
        control::add_ctrl_events(file,
                                tid,
                                2,
                                0,
                                127,
                                fnk2);
        control::add_ctrl_events(file,
                                tid,
                                3,
                                0,
                                127,
                                fnk3);
        control::add_ctrl_events(file,
                                tid,
                                3,
                                0,
                                127,
                                fnk4);


        
        char name[256];
        sprintf(name, "phrase_ver10_%03d.mid", cur);
         file.write(exportDir + name);
     }


    void test2(){
        std::string homeDir = getenv("HOME");
        std::string exportDir = homeDir + "/development/export/daito/";
        system(("mkdir -p " + exportDir).c_str());
        std::string importDir = homeDir + "/development/import/";

        // make midifile
        smf::MidiFile file;
        // set seed of random
        math::set_seed_mt();
        // add track
        int id = file.addTrack();
        // random phrase setting
        randomize::PhraseStuttingSetting setting;
        // set id
        setting.track_id = id;
        
        // duration in ticks (use MIDI::
        int num_bar = 128;
        int num_vid = 8; //
        setting.duration_in_ticks = MIDI::ONEMEASURE * num_bar;

        // divide
        setting.divisor = num_vid * num_bar;

        // dropout probability 50%
        setting.dropout.probability = 0.5;

        // repeater
        auto &repeat = setting.repeat;
        
        // repeat probability 50%
        repeat.probability = 0.2;
        repeat.time = {2, 5};
        repeat.intervals.emplace_back(MIDI::THIRTYSECOND);
        repeat.intervals.emplace_back(MIDI::TWELVETH);
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
        repeat.intervals.emplace_back(MIDI::SIXTEENTH);

        repeat.decrease_interval.min = 30;
        repeat.decrease_interval.max = 50;

        //drunk
        auto &drunk = setting.drunk;
        drunk.probability = 0.3;
        drunk.range.min = -2;
        drunk.range.max = 10;
        drunk.notes.push_back(LIVE::SNARE);
        
        // velocity function
        using vel_func = std::function<float(float)>;
        std::unordered_map<int, vel_func> vel_func_map;
        
        vel_func_map[LIVE::KICK] = [&](float t){
            return oscillator::cycle(t,num_bar * 8,0.0f);
        };
        vel_func_map[LIVE::HATCLOSE] = [&](float t){
            return oscillator::cycle(t,num_bar * 16,0.0f);
        };
        vel_func_map[LIVE::SNARE] = [&](float t){
            return oscillator::cycle(t,num_bar * 6.0f, 0.0f) * oscillator::cycle(t,num_bar * 5.0f,0.0f);
        };

//
//        vel_func_map[LIVE::KICK] = oscillator::cycle;
        
//        vector<OscFn> vel_funcs;
        
        //randomize and make phrase
        randomize::make_phrase(file, {LIVE::KICK, LIVE::HATCLOSE, LIVE::SNARE, LIVE::HATCLOSE}, setting,true);
        
        // next phrase
        // duration in ticks (use MIDI::
        num_bar = 128;
        num_vid = 6; //
        setting.duration_in_ticks = MIDI::ONEMEASURE * num_bar;

        // divide
        setting.divisor = num_vid * num_bar;

        setting.dropout.probability = 0.2;
        repeat.probability = 0.05;
        repeat.time.min = 1;
        repeat.time.max = 3;
        repeat.intervals.clear();
        repeat.intervals.emplace_back(MIDI::THIRTYSECOND);
        repeat.intervals.emplace_back(MIDI::TWENTYFOURTH);
        drunk.probability = 0.1;
        drunk.range.min = -10;
        drunk.range.max = 10;
        drunk.notes.clear();
        drunk.notes.push_back(LIVE::MARACAS);

        //randomize and make phrase
        randomize::make_phrase(file, {LIVE::MARACAS}, setting,true);

        // next phrase
        // duration in ticks (use MIDI::
        num_bar = 128;
        num_vid = 4; //
        setting.duration_in_ticks = MIDI::ONEMEASURE * num_bar;

        // divide
        setting.divisor = num_vid * num_bar;

        setting.dropout.probability = 0.1;
        repeat.probability = 0.2;
        repeat.time.min = 1;
        repeat.time.max = 3;
        repeat.intervals.clear();
        repeat.intervals.emplace_back(MIDI::SIXTH);
        repeat.intervals.emplace_back(MIDI::EIGHTH);
        drunk.probability = 0.3;
        drunk.range.min = -10;
        drunk.range.max = 10;
        drunk.notes.clear();
        drunk.notes.push_back(LIVE::CLAP);

        //randomize and make phrase
        randomize::make_phrase_with_velocity(file, {0,0,LIVE::CLAP,0}, vel_func_map, setting,true);

        
        char name[256];
        int random = math::random(1, 100000);
        sprintf(name, "phrase_ver15_%03d.mid", random);
         file.write(exportDir + name);

    }

    void test3(){
        std::string homeDir = getenv("HOME");
        std::string exportDir = homeDir + "/development/export/daito/";
        system(("mkdir -p " + exportDir).c_str());
        std::string importDir = homeDir + "/development/import/";

        // make midifile
        smf::MidiFile file;
        // set seed of random
        math::set_seed_mt();
        // add track
        int id = file.addTrack();
        // random phrase setting
        randomize::PhraseStuttingSetting setting;
        
        // velocity function
        using vel_func = std::function<float(float)>;
        std::unordered_map<int, vel_func> vel_func_map;

        // duration in ticks (use MIDI::
        int num_bar = 128;
        int num_vid = 8; //

        // repeater
        auto &repeat = setting.repeat;

        //-----------------------------------------------------
        // set KICK
        setting.track_id = id;
        
        
        num_vid = 4; //
        setting.duration_in_ticks = MIDI::ONEMEASURE * num_bar;

        // divide
        setting.divisor = num_vid * num_bar;

        // dropout probability 50%
        setting.dropout.probability = 0. ;

        
        // repeat probability 50%
        repeat.probability = 0.2;
        repeat.time = {2, 5};
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
        repeat.intervals.emplace_back(MIDI::SIXTEENTH);

        repeat.decrease_interval.min = 0;
        repeat.decrease_interval.max = 0;

        //drunk
        auto &drunk = setting.drunk;
        drunk.probability = 0.9;
        drunk.range.min = -2;
        drunk.range.max = 10;
        drunk.notes.push_back(LIVE::SNARE);
        drunk.notes.push_back(LIVE::HATCLOSE);
        drunk.notes.push_back(LIVE::CONGALOW);
        drunk.notes.push_back(LIVE::CONGAMID);
        drunk.notes.push_back(LIVE::CONGAHIGH);

        
        //randomize and make phrase
        std::vector<int> pattern = {LIVE::KICK, LIVE::KICK, 0, 0, 0, 0,0,LIVE::KICK};
        vel_func_map[LIVE::KICK] = [&](float t){
            return oscillator::cycle(t, num_bar * 8, 0.0f);
        };
        
        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting,true);
        
        
        //--------------------------------------
        // snare
        num_vid = 4; //
        setting.divisor = num_vid * num_bar;
        id = file.addTrack();
        setting.dropout.probability = 0.;
        setting.track_id = id;
        pattern = {0,0,LIVE::SNARE,0};

        vel_func_map[LIVE::SNARE] = [&](float t){
            return oscillator::cycle(t,num_bar * 6.0f, 0.0f) * oscillator::cycle(t,num_bar * 5.0f,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, true);
        //---------------------------------------
        // close hat
        num_vid = 4; //
        setting.divisor = num_vid * num_bar;

        id = file.addTrack();
        repeat.probability = 0.2;
        repeat.time = {2, 5};
        repeat.intervals.clear();
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
        repeat.intervals.emplace_back(MIDI::SIXTEENTH);

        repeat.decrease_interval.min = 3;
        repeat.decrease_interval.max = 5 ;

        setting.dropout.probability = 0.4;
        setting.track_id = id;
        pattern = {LIVE::HATCLOSE};

        vel_func_map[LIVE::HATCLOSE] = [&](float t){
            return oscillator::cycle(t,num_bar * 16,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting,true);
        //-----------------------------------------
        // conga
        num_vid = 6; //
        setting.divisor = num_vid * num_bar;

        id = file.addTrack();
        repeat.probability = 0.1;
        setting.dropout.probability = 0.2;
        setting.track_id = id;
        pattern = {LIVE::CONGALOW, LIVE::CONGAMID, LIVE::CONGAHIGH};

        vel_func_map[LIVE::CONGALOW] = [&](float t){
            return oscillator::cycle(t,num_bar * 2.0f, 0.0f) * oscillator::cycle(t,num_bar * 3.0f,0.0f);
        };
        vel_func_map[LIVE::CONGAMID] = [&](float t){
            return oscillator::cycle(t,num_bar * 3.0f, 0.0f) * oscillator::cycle(t,num_bar * 4.0f,0.0f);
        };
        vel_func_map[LIVE::CONGAHIGH] = [&](float t){
            return oscillator::cycle(t,num_bar * 5.0f, 0.0f) * oscillator::cycle(t,num_bar * 6.0f,0.0f);
        };
        
        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, false);
        //-----------------------------------------
        // hat
        id = file.addTrack();
        num_vid = 2; //
        setting.divisor = num_vid * num_bar;
        repeat.probability = 0.;
        setting.dropout.probability = 0.85;
        setting.track_id = id;
        pattern = {LIVE::HATOPEN};
        vel_func_map[LIVE::HATOPEN] = [&](float t){
            return oscillator::cycle(t,num_bar * 4.0f, 0.0f) * oscillator::cycle(t,num_bar * 6.0f,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, false);
        //-----------------------------------------
        // CLAP
        id = file.addTrack();
        num_vid = 4; //
        setting.divisor = num_vid * num_bar;
        // repeat probability 50%
        repeat.probability = 0.4;
        repeat.time = {5, 10};
        repeat.intervals.clear();
//        repeat.intervals.emplace_back(MIDI::THIRTYSECOND);
//        repeat.intervals.emplace_back(MIDI::TWELVETH);
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
//        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
//        repeat.intervals.emplace_back(MIDI::SIXTEENTH);
        repeat.intervals.emplace_back(MIDI::QUARTER);
        repeat.intervals.emplace_back(MIDI::ONEMEASURE);

        repeat.decrease_interval.min = 3;
        repeat.decrease_interval.max = 20;
        
        setting.dropout.probability = 0.5;

        setting.track_id = id;
        pattern = {LIVE::CLAP};
        vel_func_map[0,0,LIVE::CLAP,0] = [&](float t){
            return oscillator::cycle(t,num_bar * 8.0f, 0.0f) * oscillator::cycle(t,num_bar * 12.0f,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, true);
 
        //-----------------------------------------
        id = file.addTrack();
        num_vid = 16;
        randomize::arpeggiate_by_perlin(file,
                                        id,
                                        {MIDI::C_4, MIDI::D_4, MIDI::Eb_4, MIDI::F_4, MIDI::G_4, MIDI::A_4 },
                                        num_vid * num_bar,
                                        setting.duration_in_ticks);
        
        
//        control::add_ctrl_events(file,
//                                tid,
//                                1,
//                                0,
//                                127,
//                                fnk);
//        control::add_ctrl_events(file,
//                                tid,
//                                2,
//                                0,
//                                127,
//                                fnk2);
//        control::add_ctrl_events(file,
//                                tid,
//                                3,
//                                0,
//                                127,
//                                fnk3);
//        control::add_ctrl_events(file,
//                                tid,
//                                3,
//                                0,
//                                127,
//                                fnk4);
        
        //set file
        file.sortTracks();
        
        file.addTrackName(1, 1111111111, "kick");
        file.addTrackName(2, 1111111111, "snare");
        file.addTrackName(3, 1111111111, "hat");
        file.addTrackName(4, 1111111111, "perc");
        file.addTrackName(5, 1111111111, "hatopen");
        file.addTrackName(6, 1111111111, "clap");
        file.addTrackName(7, 1111111111, "arp");


        char name[256];
        int random = math::random(1, 100000);
        sprintf(name, "phrase_ver15_%03d.mid", random);
         file.write(exportDir + name);

    }

void test4(){
        std::string homeDir = getenv("HOME");
        std::string exportDir = homeDir + "/development/export/daito/";
        system(("mkdir -p " + exportDir).c_str());
        std::string importDir = homeDir + "/development/import/";

        // make midifile
        smf::MidiFile file;
        // set seed of random
        math::set_seed_mt();
        // add track
        int id = file.addTrack();
        // random phrase setting
        randomize::PhraseStuttingSetting setting;
        
        // velocity function
        using vel_func = std::function<float(float)>;
        std::unordered_map<int, vel_func> vel_func_map;

        // duration in ticks (use MIDI::
        int num_bar = 128;
        int num_vid = 8; //

        // repeater
        auto &repeat = setting.repeat;

    //drunk
    auto &drunk = setting.drunk;
    drunk.probability = 0.9;
    drunk.range.min = -2;
    drunk.range.max = 10;
    drunk.notes.push_back(LIVE::SNARE);
    drunk.notes.push_back(LIVE::HATCLOSE);
    drunk.notes.push_back(LIVE::CONGALOW);
    drunk.notes.push_back(LIVE::CONGAMID);
    drunk.notes.push_back(LIVE::CONGAHIGH);

    std::vector<int> pattern;
        //-----------------------------------------------------
        // set KICK
    {
        setting.track_id = id;
        
        num_vid = 6; //
        setting.duration_in_ticks = MIDI::ONEMEASURE * num_bar;

        // divide
        setting.divisor = num_vid * num_bar;

        // dropout probability 50%
        setting.dropout.probability = 0. ;
        
        // repeat probability 50%
        repeat.probability = 0.;
        repeat.time = {2, 5};
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
        repeat.intervals.emplace_back(MIDI::SIXTEENTH);

        repeat.decrease_interval.min = 0;
        repeat.decrease_interval.max = 0;

        
        //randomize and make phrase
        pattern = {LIVE::KICK, LIVE::KICK, LIVE::KICK, 0, 0, 0};
        vel_func_map[LIVE::KICK] = [&](float t){
            return oscillator::cycle(t, num_bar * 8, 0.0f);
        };
        
        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting,true);

    }
        
        //--------------------------------------
        //kick 2
    {
            num_vid = 6; //
            setting.duration_in_ticks = MIDI::ONEMEASURE * num_bar;

            // divide
            setting.divisor = num_vid * num_bar;

            // dropout probability 50%
            setting.dropout.probability = 0.6;

        // repeat probability 50%
            repeat.probability = 0.3;
            repeat.time = {2, 5};
            repeat.intervals.emplace_back(MIDI::TRIPLETS);
            repeat.intervals.emplace_back(MIDI::EIGHTH);
            repeat.intervals.emplace_back(MIDI::SIXTH);
            repeat.intervals.emplace_back(MIDI::SIXTEENTH);
            //randomize and make phrase
            pattern = {0, 0, 0,LIVE::KICK, LIVE::KICK, LIVE::KICK};

            repeat.decrease_interval.min = 1;
            repeat.decrease_interval.max = 3;
            randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting,true);

    }

        //--------------------------------------
        // snare
        num_vid = 4; //
        setting.divisor = num_vid * num_bar;
        id = file.addTrack();
        setting.dropout.probability = 0.;
        setting.track_id = id;
        pattern = {0,0,LIVE::SNARE,0,0,0,0,0};

    repeat.probability = 0.2;
    repeat.time = {2, 5};
    repeat.intervals.emplace_back(MIDI::TRIPLETS);
    repeat.intervals.emplace_back(MIDI::EIGHTH);
    repeat.intervals.emplace_back(MIDI::SIXTH);
    repeat.intervals.emplace_back(MIDI::SIXTEENTH);
    repeat.decrease_interval.min = 1;
    repeat.decrease_interval.max = 3;

    
        vel_func_map[LIVE::SNARE] = [&](float t){
            return oscillator::cycle(t,num_bar * 6.0f, 0.0f) * oscillator::cycle(t,num_bar * 5.0f,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, true);
        //---------------------------------------
        // close hat
        num_vid = 4; //
        setting.divisor = num_vid * num_bar;

        id = file.addTrack();
        repeat.probability = 0.2;
        repeat.time = {2, 5};
        repeat.intervals.clear();
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
        repeat.intervals.emplace_back(MIDI::SIXTEENTH);

        repeat.decrease_interval.min = 3;
        repeat.decrease_interval.max = 5 ;

        setting.dropout.probability = 0.4;
        setting.track_id = id;
        pattern = {LIVE::HATCLOSE};

        vel_func_map[LIVE::HATCLOSE] = [&](float t){
            return oscillator::cycle(t,num_bar * 16,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting,true);
        //-----------------------------------------
        // conga
        num_vid = 8; //
        setting.divisor = num_vid * num_bar;

        id = file.addTrack();
        repeat.probability = 0.1;
        setting.dropout.probability = 0.7;
        setting.track_id = id;
        pattern = {LIVE::CONGALOW, LIVE::CONGAMID, LIVE::CONGAHIGH};

        vel_func_map[LIVE::CONGALOW] = [&](float t){
            return oscillator::cycle(t,num_bar * 2.0f, 0.0f) * oscillator::cycle(t,num_bar * 3.0f,0.0f);
        };
        vel_func_map[LIVE::CONGAMID] = [&](float t){
            return oscillator::cycle(t,num_bar * 3.0f, 0.0f) * oscillator::cycle(t,num_bar * 4.0f,0.0f);
        };
        vel_func_map[LIVE::CONGAHIGH] = [&](float t){
            return oscillator::cycle(t,num_bar * 5.0f, 0.0f) * oscillator::cycle(t,num_bar * 6.0f,0.0f);
        };
        
        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, false);
        //-----------------------------------------
        // opne hat
        id = file.addTrack();
        num_vid = 2; //
        setting.divisor = num_vid * num_bar;
        repeat.probability = 0.;
        setting.dropout.probability = 0.85;
        setting.track_id = id;
        pattern = {0,0,0,0,0,0,LIVE::HATOPEN,0};
        vel_func_map[LIVE::HATOPEN] = [&](float t){
            return oscillator::cycle(t,num_bar * 4.0f, 0.0f) * oscillator::cycle(t,num_bar * 6.0f,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, false);
        //-----------------------------------------
        // CLAP
        id = file.addTrack();
        num_vid = 4; //
        setting.divisor = num_vid * num_bar;
        // repeat probability 50%
        repeat.probability = 0.4;
        repeat.time = {5, 10};
        repeat.intervals.clear();
//        repeat.intervals.emplace_back(MIDI::THIRTYSECOND);
//        repeat.intervals.emplace_back(MIDI::TWELVETH);
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
//        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
//        repeat.intervals.emplace_back(MIDI::SIXTEENTH);
        repeat.intervals.emplace_back(MIDI::QUARTER);
        repeat.intervals.emplace_back(MIDI::ONEMEASURE);

        repeat.decrease_interval.min = 3;
        repeat.decrease_interval.max = 20;
        
        setting.dropout.probability = 0.5;

        setting.track_id = id;
        pattern = {LIVE::CLAP};
        vel_func_map[0,0,LIVE::CLAP,0] = [&](float t){
            return oscillator::cycle(t,num_bar * 8.0f, 0.0f) * oscillator::cycle(t,num_bar * 12.0f,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, true);
 
        //-----------------------------------------
        id = file.addTrack();
        num_vid = 16;
        randomize::arpeggiate_by_perlin(file,
                                        id,
                                        {MIDI::C_4, MIDI::D_4, MIDI::Eb_4, MIDI::F_4, MIDI::G_4, MIDI::A_4 },
                                        num_vid * num_bar,
                                        setting.duration_in_ticks);
        
        
//        control::add_ctrl_events(file,
//                                tid,
//                                1,
//                                0,
//                                127,
//                                fnk);
//        control::add_ctrl_events(file,
//                                tid,
//                                2,
//                                0,
//                                127,
//                                fnk2);
//        control::add_ctrl_events(file,
//                                tid,
//                                3,
//                                0,
//                                127,
//                                fnk3);
//        control::add_ctrl_events(file,
//                                tid,
//                                3,
//                                0,
//                                127,
//                                fnk4);
        
        //set file
        file.sortTracks();
        
        file.addTrackName(1, 1111111111, "kick");
        file.addTrackName(2, 1111111111, "snare");
        file.addTrackName(3, 1111111111, "hat");
        file.addTrackName(4, 1111111111, "perc");
        file.addTrackName(5, 1111111111, "hatopen");
        file.addTrackName(6, 1111111111, "clap");
        file.addTrackName(7, 1111111111, "arp");


        char name[256];
        int random = math::random(1, 100000);
        sprintf(name, "phrase_ver17_%03d.mid", random);
         file.write(exportDir + name);

    }


void test5(){
        std::string homeDir = getenv("HOME");
        std::string exportDir = homeDir + "/development/export/daito/";
        system(("mkdir -p " + exportDir).c_str());
        std::string importDir = homeDir + "/development/import/";

        // make midifile
        smf::MidiFile file;
        // set seed of random
        math::set_seed_mt();
        // add track
        int id = file.addTrack();
        // random phrase setting
        randomize::PhraseStuttingSetting setting;
        
        // velocity function
        using vel_func = std::function<float(float)>;
        std::unordered_map<int, vel_func> vel_func_map;

        // duration in ticks (use MIDI::
        int num_bar = 128;
        int num_vid = 8; //

        // repeater
        auto &repeat = setting.repeat;

    //drunk
    auto &drunk = setting.drunk;
    drunk.probability = 0.9;
    drunk.range.min = -2;
    drunk.range.max = 10;
    drunk.notes.push_back(LIVE::SNARE);
    drunk.notes.push_back(LIVE::HATCLOSE);
    drunk.notes.push_back(LIVE::CONGALOW);
    drunk.notes.push_back(LIVE::CONGAMID);
    drunk.notes.push_back(LIVE::CONGAHIGH);

    std::vector<int> pattern;
    using note_w_parm = std::tuple<int, MIDI::Range, MIDI::Range>; // note vel drop
        //-----------------------------------------------------
        // set KICK
    {
        setting.track_id = id;
        
        num_vid = 8; //
        setting.duration_in_ticks = MIDI::ONEMEASURE * num_bar;

        // divide
        setting.divisor = num_vid * num_bar;

        // dropout probability 50%
        setting.dropout.probability = 0. ;
        
        // repeat probability 50%
        repeat.probability = 0.;
        repeat.time = {2, 5};
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
        repeat.intervals.emplace_back(MIDI::SIXTEENTH);

        repeat.decrease_interval.min = 0;
        repeat.decrease_interval.max = 0;

        
        //randomize and make phrase
        pattern = {LIVE::KICK, LIVE::KICK, LIVE::KICK, 0, 0, 0};
        
        vector<std::tuple<note_w_parm>> pattrs = {
            std::make_tuple(LIVE::KICK, MIDI::Range(0.9, 1.0), MIDI::Range(1.0, 1.0)),
            std::make_tuple(LIVE::KICK, MIDI::Range(0.9, 1.0), MIDI::Range(1.0, 1.0)),
            std::make_tuple(LIVE::KICK, MIDI::Range(0.1, 0.3), MIDI::Range(1.0, 1.0)),
            std::make_tuple(LIVE::KICK, MIDI::Range(0.1, 0.0), MIDI::Range(1.0, 1.0))
            
        };
//        ,
//        std::make_tuple( LIVE::KICK, MIDI::Range(0.4, 0.5), MIDI::Range(1.0, 1.0))

//        =
//        {
//            {LIVE::KICK, {0.9, 1.0}, {1.0}},
//            {LIVE::KICK, {0.5, 0.6}, {0.4, 0.7}},
//            {LIVE::MUTE, {0.5, 0.6}, {0.4, 0.7}},
//            {LIVE::MUTE, {0.5, 0.6}, {0.4, 0.7}}
//        };
        
        vel_func_map[LIVE::KICK] = [&](float t){
            return oscillator::cycle(t, num_bar * 8, 0.0f);
        };
        
        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting,true);

    }
        
        //--------------------------------------
        //kick 2
    {
            num_vid = 6; //
            setting.duration_in_ticks = MIDI::ONEMEASURE * num_bar;

            // divide
            setting.divisor = num_vid * num_bar;

            // dropout probability 50%
            setting.dropout.probability = 0.6;

        // repeat probability 50%
            repeat.probability = 0.3;
            repeat.time = {2, 5};
            repeat.intervals.emplace_back(MIDI::TRIPLETS);
            repeat.intervals.emplace_back(MIDI::EIGHTH);
            repeat.intervals.emplace_back(MIDI::SIXTH);
            repeat.intervals.emplace_back(MIDI::SIXTEENTH);
            //randomize and make phrase
            pattern = {0, 0, 0,LIVE::KICK, LIVE::KICK, LIVE::KICK};

            repeat.decrease_interval.min = 1;
            repeat.decrease_interval.max = 3;
            randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting,true);

    }

        //--------------------------------------
        // snare
        num_vid = 4; //
        setting.divisor = num_vid * num_bar;
        id = file.addTrack();
        setting.dropout.probability = 0.;
        setting.track_id = id;
        pattern = {0,0,LIVE::SNARE,0,0,0,0,0};

    repeat.probability = 0.2;
    repeat.time = {2, 5};
    repeat.intervals.emplace_back(MIDI::TRIPLETS);
    repeat.intervals.emplace_back(MIDI::EIGHTH);
    repeat.intervals.emplace_back(MIDI::SIXTH);
    repeat.intervals.emplace_back(MIDI::SIXTEENTH);
    repeat.decrease_interval.min = 1;
    repeat.decrease_interval.max = 3;

    
        vel_func_map[LIVE::SNARE] = [&](float t){
            return oscillator::cycle(t,num_bar * 6.0f, 0.0f) * oscillator::cycle(t,num_bar * 5.0f,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, true);
        //---------------------------------------
        // close hat
        num_vid = 4; //
        setting.divisor = num_vid * num_bar;

        id = file.addTrack();
        repeat.probability = 0.2;
        repeat.time = {2, 5};
        repeat.intervals.clear();
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
        repeat.intervals.emplace_back(MIDI::SIXTEENTH);

        repeat.decrease_interval.min = 3;
        repeat.decrease_interval.max = 5 ;

        setting.dropout.probability = 0.4;
        setting.track_id = id;
        pattern = {LIVE::HATCLOSE};

        vel_func_map[LIVE::HATCLOSE] = [&](float t){
            return oscillator::cycle(t,num_bar * 16,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting,true);
        //-----------------------------------------
        // conga
        num_vid = 8; //
        setting.divisor = num_vid * num_bar;

        id = file.addTrack();
        repeat.probability = 0.1;
        setting.dropout.probability = 0.7;
        setting.track_id = id;
        pattern = {LIVE::CONGALOW, LIVE::CONGAMID, LIVE::CONGAHIGH};

        vel_func_map[LIVE::CONGALOW] = [&](float t){
            return oscillator::cycle(t,num_bar * 2.0f, 0.0f) * oscillator::cycle(t,num_bar * 3.0f,0.0f);
        };
        vel_func_map[LIVE::CONGAMID] = [&](float t){
            return oscillator::cycle(t,num_bar * 3.0f, 0.0f) * oscillator::cycle(t,num_bar * 4.0f,0.0f);
        };
        vel_func_map[LIVE::CONGAHIGH] = [&](float t){
            return oscillator::cycle(t,num_bar * 5.0f, 0.0f) * oscillator::cycle(t,num_bar * 6.0f,0.0f);
        };
        
        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, false);
        //-----------------------------------------
        // opne hat
        id = file.addTrack();
        num_vid = 2; //
        setting.divisor = num_vid * num_bar;
        repeat.probability = 0.;
        setting.dropout.probability = 0.85;
        setting.track_id = id;
        pattern = {0,0,0,0,0,0,LIVE::HATOPEN,0};
        vel_func_map[LIVE::HATOPEN] = [&](float t){
            return oscillator::cycle(t,num_bar * 4.0f, 0.0f) * oscillator::cycle(t,num_bar * 6.0f,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, false);
        //-----------------------------------------
        // CLAP
        id = file.addTrack();
        num_vid = 4; //
        setting.divisor = num_vid * num_bar;
        // repeat probability 50%
        repeat.probability = 0.4;
        repeat.time = {5, 10};
        repeat.intervals.clear();
//        repeat.intervals.emplace_back(MIDI::THIRTYSECOND);
//        repeat.intervals.emplace_back(MIDI::TWELVETH);
        repeat.intervals.emplace_back(MIDI::TRIPLETS);
//        repeat.intervals.emplace_back(MIDI::EIGHTH);
        repeat.intervals.emplace_back(MIDI::SIXTH);
//        repeat.intervals.emplace_back(MIDI::SIXTEENTH);
        repeat.intervals.emplace_back(MIDI::QUARTER);
        repeat.intervals.emplace_back(MIDI::ONEMEASURE);

        repeat.decrease_interval.min = 3;
        repeat.decrease_interval.max = 20;
        
        setting.dropout.probability = 0.5;

        setting.track_id = id;
        pattern = {LIVE::CLAP};
        vel_func_map[0,0,LIVE::CLAP,0] = [&](float t){
            return oscillator::cycle(t,num_bar * 8.0f, 0.0f) * oscillator::cycle(t,num_bar * 12.0f,0.0f);
        };

        randomize::make_phrase_with_velocity(file, pattern, vel_func_map, setting, true);
 
        //-----------------------------------------
        id = file.addTrack();
        num_vid = 16;
        randomize::arpeggiate_by_perlin(file,
                                        id,
                                        {MIDI::C_4, MIDI::D_4, MIDI::Eb_4, MIDI::F_4, MIDI::G_4, MIDI::A_4 },
                                        num_vid * num_bar,
                                        setting.duration_in_ticks);
        
        
//        control::add_ctrl_events(file,
//                                tid,
//                                1,
//                                0,
//                                127,
//                                fnk);
//        control::add_ctrl_events(file,
//                                tid,
//                                2,
//                                0,
//                                127,
//                                fnk2);
//        control::add_ctrl_events(file,
//                                tid,
//                                3,
//                                0,
//                                127,
//                                fnk3);
//        control::add_ctrl_events(file,
//                                tid,
//                                3,
//                                0,
//                                127,
//                                fnk4);
        
        //set file
        file.sortTracks();
        
        file.addTrackName(1, 1111111111, "kick");
        file.addTrackName(2, 1111111111, "snare");
        file.addTrackName(3, 1111111111, "hat");
        file.addTrackName(4, 1111111111, "perc");
        file.addTrackName(5, 1111111111, "hatopen");
        file.addTrackName(6, 1111111111, "clap");
        file.addTrackName(7, 1111111111, "arp");


        char name[256];
        int random = math::random(1, 100000);
        sprintf(name, "phrase_ver17_%03d.mid", random);
         file.write(exportDir + name);

    }
void test6(){
}
    void test(){
                std::string homeDir = getenv("HOME");
                std::string exportDir = homeDir + "/development/export/daito/";
                system(("mkdir -p " + exportDir).c_str());
                std::string importDir = homeDir + "/development/import/";
        //        test_quantizer(importDir, exportDir);
        //        test_sequencer(exportDir);
        //        test_tidaloid(exportDir);
        //        test_randomize(exportDir);
        //        test_phrase(exportDir);
        int num = 100;
        for(int i=0;i<num;i++){
            test_daito::test_phrase(exportDir, i, num);

        }
        
        
    }
}

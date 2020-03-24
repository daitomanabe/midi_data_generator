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

     setting.track_id = file.addTrack();
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
    cur = file.addTrack();
    randomize::fill_by_perlin(file, cur, {LIVE::TOMLOW, LIVE::TOMMID, LIVE::TOMHIGH},
                   0.5,
                   div,
                   dur);
    cur = file.addTrack();
    randomize::arpeggiate_by_perlin(file,cur,
                                    {MIDI::C_4, MIDI::D_4, MIDI::Eb_4, MIDI::F_4, MIDI::G_4, MIDI::A_5 },
                                    div,
                                    dur);
    
    char name[256];
    sprintf(name, "phrase_ver7_%03d.mid", cur);
     file.write(exportDir + name);
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

//
//  test_2bit.cpp
//  midi_data_generator
//
//  Created by 2bit on 2020/03/11.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#include "test_2bit.hpp"
#include "Randomize.h"
#include "Quantize.h"
#include "Sequencer.h"

namespace test_2bit {
    void test_sequencer(const std::string &exportDir) {
        smf::MidiFile file;
        file.setTPQ(120);
        file.addTempo(0, 0, 120);
        
        auto phrase = sequencer::Phrase(
            {46, 43, 43, 42, 45},
            [](float t) { return t * t; },
            [](float from, float to) {
                return sequencer::notePerDiv(16)(from, to) && 0.2 < math::random(1.0);
            });
        
        sequencer::Sequence()
            .play(sequencer::PlayMode::Linear(1), phrase)
            .then(sequencer::PlayMode::Repeat(3), {40, 41, 43, 42, 41}, [](float t) { return t * t; }, 16)
            .then(sequencer::PlayMode::Pingpong(1), phrase)
            .write(file);
        file.write(exportDir + "sequence_test.mid");
    }
    
    void test() {
        std::string homeDir = getenv("HOME");
        std::string exportDir = homeDir + "/development/export/";
        std::string importDir = homeDir + "/development/import/";
        
        std::string src_file = importDir + "02.mid";
        
        smf::MidiFile file;
        file.read(src_file);
        auto curve = [](float t) { return std::sin(t * M_PI * 0.5f); };
        quantize::per_ticks(file,
                            curve,
                            96);
        file.write(exportDir + "02_cos_96.mid");
        
        file.read(src_file);
        quantize::per_ticks(file,
                            curve,
                            48);
        file.write(exportDir + "02_cos_48.mid");
   
        file.read(src_file);
        quantize::per_ticks(file,
                            curve,
                            32);
        file.write(exportDir + "02_cos_32.mid");
        
        test_sequencer(exportDir);
    }
}

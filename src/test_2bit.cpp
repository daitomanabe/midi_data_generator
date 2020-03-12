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
#include "Tidaloid.h"

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
    
    void test_quantizer(const std::string &importDir, const std::string &exportDir) {
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
    }
    
    void test_tidaloid(const std::string &exportDir) {
        std::string sequence = "[[bd sd1 bd, _ sd2 _ sd2], hh hh [ho hh] hh hh hh [ho hh] [hh hh]]";
        smf::MidiFile file;
        int track_id = file.addTrack();
        
        std::map<std::string, int> note_table = {
            {"bd",  MIDI::C_1},
            {"sd1", MIDI::D_1},
            {"sd2", MIDI::Ds_1},
            {"hh",  MIDI::Fs_1},
            {"ho",  MIDI::Gs_1}
        };
        
        Tidaloid::eval(file, track_id, sequence, note_table);
        file.write(exportDir + "tidaloid_test.mid");
    }
    
    void test() {
        std::string homeDir = getenv("HOME");
        std::string exportDir = homeDir + "/development/export/";
        std::string importDir = homeDir + "/development/import/";
        test_quantizer(importDir, exportDir);
        test_sequencer(exportDir);
        test_tidaloid(exportDir);
    }
}

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
        
        auto phrase = sequencer::GeneratablePhrase(
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
        smf::MidiFile file;
        std::string sequence = "[[bd sd1 bd, _ sd2 _ sd2], hh hh [ho hh] hh hh hh [ho hh] [hh hh]]";
        int track_id = file.addTrack();
        
        std::map<std::string, int> note_table = {
            {"bd",  MIDI::C_1},
            {"sd1", MIDI::D_1},
            {"sd2", MIDI::Ds_1},
            {"hh",  MIDI::Fs_1},
            {"ho",  MIDI::Gs_1}
        };
        
        tidaloid::eval(file, track_id, sequence, note_table);
        
        track_id = file.addTrack();
        sequence = "[bd bd bd bd], [[ho hh] [ho hh] [ho ho hh]], [sd1 sd2 sd1 sd2 sd1]";
        tidaloid::eval(file, track_id, sequence, note_table);
        file.write(exportDir + "tidaloid_test.mid");
    }
    
    void test_randomize(std::string exportDir) {
        smf::MidiFile file;
        int track = file.addTrack();
        randomize::fill_by_perlin(file,
                                  track,
                                  {MIDI::C_1, MIDI::D_1, MIDI::Fs_1, MIDI::Gs_1},
                                  0.5f,
                                  64,
                                  4 * MIDI::ONEMEASURE);
        track = file.addTrack();
        randomize::arpeggiate_by_perlin(file,
                                        track,
                                        {MIDI::C_4, MIDI::E_4, MIDI::G_4, MIDI::B_4},
                                        64,
                                        4 * MIDI::ONEMEASURE);
        file.write(exportDir + "perlin.mid");
    }
    
    void test_phrase(std::string exportDir) {
        math::set_seed_fast();
        smf::MidiFile file;
        randomize::PhraseStuttingSetting setting;
        setting.duration_in_ticks = MIDI::ONEMEASURE * 4;
        setting.track_id = file.addTrack();
        setting.dropout.probability = 0.25f;
        auto &repeat = setting.repeat;
        repeat.probability = 0.7f;
        repeat.time = 8;
        repeat.interval = MIDI::EIGHTH;
        repeat.decrease_interval.min = 5;
        repeat.decrease_interval.max = 8;
        randomize::make_phrase(file,
                               {MIDI::C_3, MIDI::E_3, MIDI::Gs_3},
                               setting);
        
        //
        setting.track_id = file.addTrack();
        setting.dropout.probability = 0.1f;
        repeat.time = 16;
        repeat.interval = MIDI::QUARTER;
        repeat.decrease_interval.min = 10;
        repeat.decrease_interval.max = 30;
        repeat.probability = [](float t) { return t * 0.5f + 0.5f; };

        randomize::make_phrase(file,
                               {MIDI::C_1, MIDI::E_1, MIDI::Gs_1, MIDI::B_1},
                               setting);

        file.write(exportDir + "phrase.mid");
    }
    
    void test() {
        std::string homeDir = getenv("HOME");
        std::string exportDir = homeDir + "/development/export/2bit/";
        system(("mkdir -p " + exportDir).c_str());
        std::string importDir = homeDir + "/development/import/";
        test_quantizer(importDir, exportDir);
        test_sequencer(exportDir);
        test_tidaloid(exportDir);
        test_randomize(exportDir);
        test_phrase(exportDir);
    }
}

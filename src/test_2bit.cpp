//
//  test_2bit.cpp
//  midi_data_generator
//
//  Created by 2bit on 2020/03/11.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#include "test_2bit.hpp"

#include "Effect.h"
#include "Randomize.h"
#include "Quantize.h"
#include "Sequencer.h"
#include "Tidaloid.h"
#include "MidiFileUtils.h"

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
        setting.divisor = 16;
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
        setting.divisor = 64;
        setting.track_id = file.addTrack();
        setting.dropout.probability = 0.1f;
        repeat.time = 3;
        repeat.interval = MIDI::SIXTEENTH;
        repeat.decrease_interval.min = 10;
        repeat.decrease_interval.max = 30;
        repeat.probability = [](float t) { return t * 0.5f + 0.5f; };
        
        auto &drunk = setting.drunk;
        drunk.probability = 0.7f;
        drunk.range.min = -10;
        drunk.range.max = 10;
        drunk.notes.push_back(MIDI::E_1);
        drunk.notes.push_back(MIDI::Gs_1);

        randomize::make_phrase(file,
                               {MIDI::C_1, MIDI::E_1, MIDI::Gs_1, MIDI::B_1},
                               setting);

        file.write(exportDir + "phrase.mid");
    }
    
    void test_midi_phrase(std::string importDir, std::string exportDir) {
        smf::MidiFile file;
        smf::MidiFile output;
        file.read(importDir + "basic.mid");
        sequencer::MIDIFilePhrase phrase{file, 0, {0, file.getTPQ() * 4}};
        sequencer::Sequence()
            .play(sequencer::PlayMode::Pingpong(4), phrase)
            .write(output);
        output.write(exportDir + "basic_pingpon4.mid");
    }
    
    void test_stutter_effect(std::string exportDir) {
        smf::MidiFile file;
        file.setTPQ(120);
        int track_id = file.addTrack();
        std::string tidal_seq = "[bd sd bd sd] x4, [hh x8] x4";
        tidaloid::eval(file, tidal_seq, {{"bd", MIDI::C_1}, {"sd", MIDI::E_1}, {"hh", MIDI::Fs_1}, {"ho", MIDI::Gs_1}}, MIDI::Setting(track_id, MIDI::ONEMEASURE * 4, 0));
        file.write(exportDir + "stutter_orig.mid");
        MIDI::Effect::StutterParameter param;
        param.div_ticks = file.getTPQ();
        param.repeat.decrease_interval.min = 3;
        param.repeat.decrease_interval.max = 8;
        param.repeat.probability = 0.7f;
        MIDI::Effect::stutter(file, track_id, param);
        file.write(exportDir + "stuttered.mid");
        
        file.read(exportDir + "stutter_orig.mid");
        MIDI::Effect::stutter_per_note(file, track_id, param);
        file.write(exportDir + "stuttered_per_note.mid");
    }
    
    void test_complex_pattern(std::string exportDir) {
        smf::MidiFile file;
        file.setTPQ(120);
        int track_id = file.addTrack();
//        std::string tidal_seq = "[bd sd] x3 [bd [sd sd]], [hh x2 [ho hh] hh] x2";
        std::string tidal_seq = "[bd sd] x3 [bd [sd sd]], [hh x2 [ho hh] hh] x2";
        tidaloid::eval(file, track_id, tidal_seq, {{"bd", MIDI::C_1}, {"sd", MIDI::E_1}, {"hh", MIDI::Fs_1}, {"ho", MIDI::Gs_1}});
        
        smf::MidiFile perlin;
        perlin.setTPQ(120);
        int perlin_track = perlin.addTrack();
        randomize::fill_by_perlin(perlin,
                                  perlin_track,
                                  {MIDI::C_1, MIDI::E_1, MIDI::Fs_1, MIDI::Gs_1},
                                  0.5f,
                                  16,
                                  MIDI::ONEMEASURE);
        
        sequencer::MIDIFilePhrase phrase1{file, track_id, {0, file.getTPQ() * 4}};
        sequencer::MIDIFilePhrase phrase2{perlin, perlin_track, {0, perlin.getTPQ() * 4}};
        
        sequencer::Sequence()
            .play(sequencer::PlayMode::Linear(4), phrase1)
            .play(sequencer::PlayMode::Pingpong(4), phrase1)
            .play(sequencer::PlayMode::Pingpong(4), phrase2)
            .write(file, MIDI::Setting{track_id, MIDI::ONEMEASURE, 0});
        
        quantize::per_ticks(file, [](float v) { return std::pow(v, 0.7f); }, {MIDI::AllTrack, MIDI::QUARTER, 0});
        
        file.write(exportDir + "complex.mid");
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
        test_midi_phrase(importDir, exportDir);
        test_stutter_effect(exportDir);
        test_complex_pattern(exportDir);
    }
}

//
//  test_daito_2.cpp
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/07/19.
//  Copyright © 2020 Daito Manabe. All rights reserved.
//

#include "test_daito_2.hpp"

#include "Randomize.h"
//#include "Quantize.h"
#include "Effect.h"
#include "Sequencer.h"
#include "Tidaloid.h"
#include "LiveConstants.h"
#include "Control.h"
#include "Oscillator.h"
#include "OFUtils.hpp"
using namespace std;
namespace test_daito {
    void test10(){
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
        randomize::DetailedSetting setting;
        auto & notes = setting.notes;
        //kick
        {
            randomize::detailed_note nd;
            nd.note << LIVE::KICK;
            nd.dropout.probability = 0.5;
            nd.drunk.probability = 0.5;
            nd.drunk.range = {1, MIDI::EIGHTH};
            nd.repeat.probability = 0.5;
            nd.repeat.intervals = {MIDI::SIXTEENTH, MIDI::EIGHTH};
            nd.repeat.note_duration = MIDI::SIXTEENTH;
            nd.repeat.time = {1,5};
            notes.emplace_back(nd);
        }
        //snare
        {
            randomize::detailed_note nd;
            nd.note << LIVE::SNARE;
            nd.dropout.probability = 0.5;
            nd.drunk.probability = 0.5;
            nd.drunk.range = {1, MIDI::EIGHTH};
            nd.repeat.probability = 0.5;
            nd.repeat.intervals = {MIDI::SIXTEENTH, MIDI::EIGHTH};
            nd.repeat.note_duration = MIDI::SIXTEENTH;
            nd.repeat.time = {1,5};
            notes.emplace_back(nd);
        }

        
        
            // velocity function
            using vel_func = std::function<float(float)>;
            std::unordered_map<int, vel_func> vel_func_map;

            // duration in ticks (use MIDI::
            int num_bar = 128;
            int num_vid = 8; //
//
//            // repeater
//            auto &repeat = setting.repeat;
//
//        //drunk
//        auto &drunk = setting.drunk;
//        drunk.probability = 0.9;
//        drunk.range.min = -2;
//        drunk.range.max = 10;
//        drunk.notes.push_back(LIVE::SNARE);
//        drunk.notes.push_back(LIVE::HATCLOSE);
//        drunk.notes.push_back(LIVE::CONGALOW);
//        drunk.notes.push_back(LIVE::CONGAMID);
//        drunk.notes.push_back(LIVE::CONGAHIGH);

    }

void test11_stutter(int cur){
    std::string homeDir = getenv("HOME");
    std::string exportDir = homeDir + "/development/export/daito/stutter/";
    system(("mkdir -p " + exportDir).c_str());
    std::string importDir = homeDir + "/development/import/";

    smf::MidiFile file;
    file.setTPQ(120);
    int track_id = file.addTrack();
    std::string tidal_seq = "[bd sd bd sd] x4, [hh x8] x4";
    tidaloid::eval(file, tidal_seq, {{"bd", MIDI::C_1}, {"sd", MIDI::E_1}, {"hh", MIDI::Fs_1}, {"ho", MIDI::Gs_1}}, MIDI::Setting(track_id, MIDI::ONEMEASURE * 8, 0));
    
    string base_name = ofGetTimestampString();
    
    file.write(exportDir + "org_stutter.mid");
    MIDI::Effect::StutterParameter param;
    param.div_ticks = file.getTPQ();
    param.repeat.decrease_interval.min = 3;
    param.repeat.decrease_interval.max = 8;
    param.repeat.probability = 0.7f;
    MIDI::Effect::stutter(file, track_id, param);
    file.write(exportDir + "stuttered" + ofToString(cur) + ".mid");
    file.read(exportDir + "org_stutter" + ofToString(cur) + ".mid");
    MIDI::Effect::stutter_per_note(file, track_id, param);
    file.write(exportDir + "per_note_stuttered"+ ofToString(cur) + ".mid");
    
}

void test12_complex(){
            std::string homeDir = getenv("HOME");
            std::string exportDir = homeDir + "/development/export/daito/";
            system(("mkdir -p " + exportDir).c_str());
            std::string importDir = homeDir + "/development/import/";

        smf::MidiFile file;
//            int track_id = file.addTrack();
//    //        std::string tidal_seq = "[bd sd] x3 [bd [sd sd]], [hh x2 [ho hh] hh] x2";
//            std::string tidal_seq = "[bd sd] x3 [bd [sd sd]], [hh x2 [ho hh] hh] x2";
//            tidaloid::eval(file, track_id, tidal_seq, {{"bd", MIDI::C_1}, {"sd", MIDI::E_1}, {"hh", MIDI::Fs_1}, {"ho", MIDI::Gs_1}});
//            file.write(exportDir + "stuttered_2_org" + ofGetTimestampString() + ".mid");

    string fname = "/Users/daitomanabe/Music/Files/Loopmasters19/DopeSONIX - Boom Bap Classics [MIDI]/DopeSONIX - Boombap Classics/dopesonix.com (Aqua & Joe 3h Weinberger) Jay-Z - My 1st Song 78.5.mid";
    file.read(fname);
    int track_id = 1;
    cout << file.getTrackCountAsType1() << endl;
    cout << file.getNumTracks() << endl;
    cout << file.getFileDurationInTicks() << endl;
    cout << file.getTPQ() << endl;
    file.setTPQ(120);
    cout << file.getTPQ() << endl;

    MIDI::Effect::StutterParameter param;
    param.div_ticks = file.getTPQ();
    param.repeat.decrease_interval.min = 3;
    param.repeat.decrease_interval.max = 8;
    param.repeat.probability = 0.7f;
    MIDI::Effect::stutter(file, MIDI::AllTrack, param);

//
//    smf::MidiFile perlin;
//    perlin.setTPQ(120);
//    int perlin_track = perlin.addTrack();
//    randomize::fill_by_perlin(perlin,
//                              perlin_track,
//                              {MIDI::C_1, MIDI::E_1, MIDI::Fs_1, MIDI::Gs_1},
//                              0.5f,
//                              16,
//                              MIDI::ONEMEASURE);
////
//            sequencer::MIDIFilePhrase phrase1{file, track_id, {0, file.getTPQ() * 4}};
//            sequencer::MIDIFilePhrase phrase2{perlin, perlin_track, {0, perlin.getTPQ() * 4}};
//
//            sequencer::Sequence()
//                .play(sequencer::PlayMode::Linear(4), phrase1)
//                .play(sequencer::PlayMode::Pingpong(4), phrase1)
//                .play(sequencer::PlayMode::Pingpong(4), phrase2)
//                .write(file, MIDI::Setting{track_id, MIDI::ONEMEASURE, 0});

//            quantize::per_ticks(file, [](float v) { return std::pow(v, 0.7f); }, {MIDI::AllTrack, MIDI::QUARTER, 0});

            file.write(exportDir + "complex_" + ofGetTimestampString() + ".mid");
}

};

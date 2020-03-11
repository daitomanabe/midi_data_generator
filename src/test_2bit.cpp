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

namespace test_2bit {
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
    }
}

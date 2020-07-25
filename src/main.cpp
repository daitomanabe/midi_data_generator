//
//  main.cpp
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/03/07.
//

#include "test_2bit.hpp"
#include "test_daito.hpp"
#include "test_daito_2.hpp"

#include "Oscillator.h"
#include <iostream>
#include <random>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char** argv) {
//    test_2bit::test();
//    for(int i=0;i<1;i++){
//        test_daito::test11_stutter(i);
//    }
    test_daito::test12_complex();
    return 0;
}


//
//  main.cpp
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/03/07.
//

#include <iostream>
#include "MidiFile.h"
#include "Easing.h"
#include <unistd.h>
using namespace std;
using namespace smf;
using namespace choreograph;

#define NOTE_OFF 0x80;
#define NOTE_ON 0x90;
#define POLYPHONIC_PRESSURE 0xA0:
#define CONTROL_CHANGE 0xB0:
#define PROGRAM_CHANGE 0xC0:
#define CHANNEL_PRESSURE 0xD0:
#define PITCHBEND 0xE0:

#define HIGH_HAT    59
#define SNARE       38
#define BASS_DRUM   41
#define ONEMEASURE 480
#define QUARTER   120        /* ticks per quarter note */
#define EIGHTH 60
#define SIXTEENTH 30
#define THIRTYSECOND 15

//ChoreographのEasing.hを使用
typedef std::function<float (float)> EaseFn;
 
/* ---trackにnote eventを追加する物--- */
void add_periodic(MidiFile& midifile,
                  int track_num,
                  int note = HIGH_HAT,
                  int velocity = 127,
                  const EaseFn &ease_fn = &easeNone,
                  int div = 8,
                  int duration_in_ticks = ONEMEASURE){
    for(int i=0; i<div ;i++){
        
        float step = 1.0 / div * i;
        int action_time = ease_fn(step) * duration_in_ticks;
        vector<uchar> midievent;
        midievent.resize(3);
        midievent[0] = NOTE_ON;
        midievent[1] = note;
        midievent[2] = velocity;
        midifile.addEvent(track_num, action_time, midievent);
        action_time += step;
        midievent[0] = NOTE_OFF;
        midievent[1] = note;
        midifile.addEvent(track_num, action_time, midievent);
    }
}


//trackにnote eventをランダムに追加
void add_notes(MidiFile& midifile,
                const vector<string> pattern,// {o,x,o,o}みたいなやつ, ランダムの中にもパターンが欲しい
                const vector<int> notes,//この中からランダムに選ぶ
                int min_velocity,
                int max_velocity,
                int interval_in_ticks, // interval between each events
                float probability = 1.0, // 0.0 ~ 1.0 0.0 is always notes[0]
                float dropout = 0., // 0.0 is always add, 1.0 don't add, 0.5 = 50% add
                float repeat_probability = 0.f, //
                int repeat_time = 10, //
                float repeat_interval = THIRTYSECOND, //
                float repeat_decrease_interval = 10// in ticks リピートのたびにリピートの感覚が短くなる
                );




/*--- midiファイルを読み込んでエフェクトをかけるようなもの---*/
// repetitive : Ableton Liveのbeat repeaterの様な物
// stutter   Izotopeのstutterの様な物

int main(int argc, char** argv) {
   MidiFile outputfile;        // create an empty MIDI file with one track
//       outputfile.absoluteTicks(); // time information stored as absolute time
//       outputfile.setTicksPerQuarterNote(QUARTER);
   int track = outputfile.addTrack();
    int velocity = 127;
    
    add_periodic(outputfile,
                 track,
                 SNARE,
                 127,
                 easeInOutQuad,
                 16*32,
                 ONEMEASURE * 16);

   outputfile.sortTracks();         // make sure data is in correct order

    // change path if you need
    const char *homeDir = getenv("HOME");
    string output_path = string(homeDir) + "/development/export/test.mid";
    outputfile.write(output_path);

   return 0;
}


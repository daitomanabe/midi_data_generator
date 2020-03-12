//
//  main.cpp
//  midi_data_generator
//
//  Created by Daito Manabe on 2020/03/07.
//

#include <iostream>
#include <random>
#include "MidiFile.h"
#include "Easing.h"
#include <unistd.h>
#include "MathConstants.h"
#include <cstdlib>

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
#define BASS_DRUM   36
 
#define ONEMEASURE 480
#define QUARTER   120        /* ticks per quarter note */
#define EIGHTH 60
#define SIXTEENTH 30
#define THIRTYSECOND 15

//ChoreographのEasing.hを使用
typedef std::function<float (float)> EaseFn;
 
/* ---trackにnote eventを追加する物--- */
inline void add_note_event(MidiFile& midifile,
                           int track_id,
                           int note,
                           int velocity,
                           int duration,
                           int action_time){
    vector<uchar> midievent;
    midievent.resize(3);
    midievent[0] = NOTE_ON;
    midievent[1] = note;
    midievent[2] = velocity;
    midifile.addEvent(track_id, action_time, midievent);
    action_time += duration;
    midievent[0] = NOTE_OFF;
    midievent[1] = note;
    midifile.addEvent(track_id, action_time, midievent);
}

void add_periodic(MidiFile& midifile,
                  int track_id,
                  int note = HIGH_HAT,
                  int velocity = 127,
                  const EaseFn &ease_fn = &easeNone,
                  int div = 8,
                  int duration_in_ticks = ONEMEASURE,
                  int start_point_in_ticks = 0){
    for(int i=0; i<div ;i++){
        float current_pos_norm = i / static_cast<float>(div);
        int action_time = ease_fn(current_pos_norm) * duration_in_ticks + start_point_in_ticks;
        vector<uchar> midievent;
        midievent.resize(3);
        midievent[0] = NOTE_ON;
        midievent[1] = note;
        midievent[2] = velocity;
        midifile.addEvent(track_id, action_time, midievent);
        action_time += current_pos_norm * duration_in_ticks;
        midievent[0] = NOTE_OFF;
        midievent[1] = note;
        midifile.addEvent(track_id, action_time, midievent);
    }
}

// tidalcycleぽい感じで o + x -を使ってパターンを作る
void add_pattern(MidiFile& midifile,
                 int track_id,
                 vector<string>beat_pattern,// {o,+, -,x,o,x,o}とか
                vector<int>notes, // this should be three
                int velocity,
                const EaseFn &ease_fn = &easeNone,
                int duration_in_ticks = ONEMEASURE
                        ){
    vector<uchar> midievent;
    midievent.resize(3);

    int div = beat_pattern.size() ;
    
    for(int i=0; i<div; i++){
        float current_pos_norm = i / static_cast<float>(div);
        int action_time = ease_fn(current_pos_norm) * duration_in_ticks;
        if(beat_pattern[i] == "-"){
            continue;
        }

        int note;
        if(beat_pattern[i] == "o"){
            note = notes[0];
        }
        else if(beat_pattern[i] == "x"){
            note = notes[1];
        }
        else if(beat_pattern[i] == "+"){
            note = notes[2];
        }
        else{
            note = notes[0];
        }
        midievent[0] = NOTE_ON;
        midievent[1] = note;
        midievent[2] = velocity;
        midifile.addEvent(track_id, action_time, midievent);
        action_time += current_pos_norm * duration_in_ticks - 1;
        midievent[0] = NOTE_OFF;
        midievent[1] = note;
        midifile.addEvent(track_id, action_time, midievent);
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
){
    
}

//

void addPitchBendTest(MidiFile& midifile,
int track_id,
int ctrl_ch = 1,
int velocity = 127,
const EaseFn &ease_fn = &easeNone,
int div = 8,
int duration_in_ticks = ONEMEASURE)
{

}

//pitch bend , ctrl 用
//tri,sin,cos,noise,
//周波数はtick単位
namespace oscillator{
    inline float cycle(float t, float freq, float phase){
        // add 0.5 to time for starting from zero value.
        return (cos(TWO_PI * (t + 0.5) * freq + phase) + 1.) * 0.5;
    }
    //train~
    //WIP
    inline float train(float t, float freq, float duty_ratio, float phase){
        float val = 0.;
        return val;
    }
    // tri~
    inline float tri(float t, float freq, float duty_cycle = 0.5f) {
        return std::fabs(std::fmod(std::fabs(t * freq), 1) - 0.5f) * 2.0f;
    }
    // phasor~
    inline float phasor(float t, float freq) {
        float v = t * freq;
        return (v < 0.0) ? fmod(1.0 - fmod(-v, 1.0), 1.0) : std::fmod(v, 1.0);
    }
};

typedef std::function<float (float, float , float)> OscFn;


inline void add_ctrl_event(MidiFile& midifile, int track_id,
                           int channel,
                           int val,
                           int action_time){
    int ctrl_change = 0xB0;
    vector<uchar> midievent;
    midievent.resize(3);
    midievent[0] = ctrl_change;
    midievent[1] = channel;
    midievent[2] = val;
    midifile.addEvent(track_id, action_time, midievent);
}


inline void add_ctrl_events(MidiFile& midifile,
                            int track_id,
                            int channel,
                            float min_val,
                            float max_val,
                            const EaseFn & fn,
                            int duration_in_ticks = ONEMEASURE,
                            float resolution_in_ticks = 5
                            )
{
    int div = duration_in_ticks / resolution_in_ticks;
    int action_time = 0;
    
    for(int i=0; i<div; i++){
        float current_pos_norm = i / static_cast<float>(div);
        uchar val = static_cast<uchar>(fn(current_pos_norm) * 128);
        add_ctrl_event(midifile, track_id, channel, val, action_time);
        action_time += resolution_in_ticks;
    }
}


typedef std::function<bool (float)> CondFn;// conditional function

inline void add_notes_with_condition(MidiFile& midifile,
                                     int track_id,
                                     vector<int> random_note,
                                     vector<int> random_velocity,
                                     EaseFn fn,
                                     CondFn cn,
                                     int note_duration = EIGHTH,
                                     int duration_in_ticks = ONEMEASURE)
{
    for(int i=0;i<duration_in_ticks;i++){
        float current_pos_norm = i / static_cast<float>(duration_in_ticks);
        float val = fn(current_pos_norm);
        if(cn(val)){
            add_note_event(midifile,
                           track_id,
                           random_note[0],
                           random_velocity[0],
                           note_duration,
                           i
                           );
        }
    }
}
    /*--- midiファイルを読み込んでエフェクトをかけるようなもの---*/
// repetitive : Ableton Liveのbeat repeaterの様な物
// stutter   Izotopeのstutterの様な物

int main(int argc, char** argv) {
    
   MidiFile outputfile;        // create an empty MIDI file with one track
//       outputfile.absoluteTicks(); // time information stored as absolute time
    outputfile.setTicksPerQuarterNote(QUARTER);
    int track = outputfile.addTrack();
    int velocity = 127;

    
    //control value
//    int length = 32;
//    std::function<float(float)> cycle_4x7x11hz = [=](float t)->float{return
//        oscillator::cycle(t, 4. * length, 0.)
//        * oscillator::cycle(t, 7. * length, 0.)
//        * oscillator::cycle(t, 11. * length, 0.);};
//
//    add_ctrl_events(outputfile,
//                    track,
//                    1,
//                    0.,
//                    1.,
//                    cycle_4x7x11hz,
//                    ONEMEASURE * length,
//                    1);
//
//
//    std::function<bool(float)> cond_func_1 = [](float t)->bool{
//        if(t==0.0 || t == 0.5 || t == 1.0){
//            return true;
//        }else{
//            return false;
//        }
//    };
//
//
//    add_notes_with_condition(outputfile, track, {BASS_DRUM}, {127}, cycle_4x7x11hz, cond_func_1
//                             ,EIGHTH,
//                             ONEMEASURE * length);
//
//    std::function<bool(float)> cond_func_2 = [](float t)->bool{
//        if(t==0.25 || t == 0.75){
//            return true;
//        }else{
//            return false;
//        }
//    };
//
//    track = outputfile.addTrack();
//
//
//    add_notes_with_condition(outputfile, track, {SNARE}, {127}, cycle_4x7x11hz, cond_func_2
//                             ,EIGHTH,
//                             ONEMEASURE * length);

//    track = outputfile.addTrack();

    int position = 0;
    for(int i=0;i<16;i++){
        add_periodic(outputfile,
                     track,
                     BASS_DRUM,
                     velocity,
                     easeInOutCirc,
                     16,
                     ONEMEASURE,
                     i * ONEMEASURE * 0.6666);
    }



//    track = outputfile.addTrack();
//    add_pattern(outputfile,
//                        track,
//                        {"o",  "o", "x", "o", "-", "o", "-", "o"},
//                        {BASS_DRUM, SNARE, HIGH_HAT},
//                        127,
//                        easeInOutQuad,
//                       ONEMEASURE * 2);
    
//   add_pattern(outputfile,
//                       track,
//                       {"+",  "-", "+", "-", "-", "+", "-", "+"},
//                       {BASS_DRUM, SNARE, HIGH_HAT},
//                       127,
//                       easeInOutQuad,
//                      ONEMEASURE * 2);
   outputfile.sortTracks();         // make sure data is in correct order

    // change path if you need
    std::string homeDir = getenv("HOME");
    std::string exportDir = homeDir + "/development/export/";
    std::system(("mkdir -p " + exportDir).c_str());
    string output_path = exportDir + "test.mid";
    outputfile.write(output_path);
    return 0;
}


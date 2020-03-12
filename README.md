# midi_data_generator
making beats with C++



## Sequencer

```c++
int tpq = 120;
smf::MidiFile file;
file.setTPQ(tpq);
int track_id = file.addTrack();
auto phrase = sequencer::Phrase(
    {MIDI::C_1, MIDI::D_1, MIDI::C_1, MIDI::Fs_1, MIDI::Gs_1},
    [](float t) { return t * t; },
    [](float from, float to) {
        return sequencer::notePerDiv(16)(from, to) && 0.2 < math::random(1.0);
    });

sequencer::Sequence()
  	// Repeat == Linear
    .play(sequencer::PlayMode::Linear(1), phrase)
  	// or add directly
    .then(sequencer::PlayMode::Repeat(3), {40, 41, 43, 42, 41}, [](float t) { return std::cos(t * M_TWO_PI) * 0.5 + 0.5; }, 16)
	  // Pingpong(3) == Linear(1), Reverse(1), Linear(1),
    .then(sequencer::PlayMode::Pingpong(3), phrase)
    .then(sequencer::PlayMode::Reverse(1), phrase)
    .write(file, tpq, track_id);
file.write(file_path);

```



## Tidaloid

```c++
smf::MidiFile file;
int track = file.addTrack();
std::string tidal_sequence = "bd bd bd bd, _ sd _ [sd _ _ sd], hh hh hh hh hh hh hh [ho hh]";
int loop_duration_in_ticks = MIDI::ONEMEASURE;
std::map<std::string, int> note_table = {
  {"bd", MIDI::C_1},
  {"sd", MIDI::D_1},
  {"hh",  MIDI::Fs_1},
  {"ho",  MIDI::Gs_1}
}; // "_" for mute
tidaloid::eval(file, track, tidal_sequence, note_table, ONEMEASURE);
file.write(file_path);
```


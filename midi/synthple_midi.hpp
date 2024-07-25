#pragma once

#include <string>
#include <vector>

#include <midifile/MidiFile.h>
#include <spdlog/spdlog.h>

#include <synthple_globals.hpp>

namespace synthple::midi {

    enum MidiEventType{
        NOTE_ON,
        NOTE_OFF,
        END_OF_SEQUENCE,
        OTHER
    };



    struct MidiNote {

        MidiNote() { note = NoteKey::A; octave = 4; note_value = toString(); }
        MidiNote( NoteKey nk, int o ){ note = nk; octave = o; note_value = toString(); }

        NoteKey note;
        std::string note_value;
        ushort octave; // 0 to 9

        std::string toString();
    };



    struct MidiEventWrapper {
        
        MidiEventWrapper(){}
        MidiEventWrapper( smf::MidiEvent* mev );
        MidiEventWrapper(
            MidiEventType _t,
            MidiNote _n,
            int _ticks
        );

        MidiEventType type;
        MidiNote note;
        int ticks;

        
        std::string toString();

        private:

            MidiEventType _getMidiEventTypeFromCode( int eventCode );
            MidiNote _getMidiEventNoteFromCode( int noteCode );
            NoteKey _intToNote(int noteNumber);
    };



    class MonophonicMidiFileReader {

        int _id;
        std::string _file_path;
        std::vector<MidiEventWrapper> _midi_events;
        std::shared_ptr<spdlog::logger> _logger;

        std::vector<MidiEventWrapper>::iterator _curr_evt, 
                                            _next_evt_aux;

        int _end_of_sequence__tick,
            _total_ticks;

        short _length_bars,
            _beats_per_bar;
        
        float _tempo_bpm,
              _ticks_per_beat,
              _beat_duration_s,
              _tick_duration_s,
              _last_time_aux_s,
              _duration_s;

        void _populateMidiEvents(smf::MidiFile &file);
        int _t_secs__toTicks( float tsecs);

        public:

            MonophonicMidiFileReader(
                int readerid,
                std::string filePath,
                float tempobpm, 
                float dts,
                short lengthbars,
                short beatsperbar );

            MidiNote *getStateAt_Time_s(float t_s);
            MidiEventWrapper *getActiveMidiEventAt_Time_s(float t_s);
    };
}
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

        std::string _file_path;
        std::vector<MidiEventWrapper> _midi_events;
        std::shared_ptr<spdlog::logger> _logger;

        // stuff to help reading a file
        int _current_tick,
            _current_midi_evt_index,
            _ticks_per_loop;

        short _length_bars;
        
        float _dt_s,
              _current_time_s,
              _tempo_bpm,
              _ticks_per_beat,
              _tick_duration_s,
              _duration_s;

        bool _is_silent;


        MidiNote _active_note;

        void _populateMidiEvents(smf::MidiFile &file);
        int _getMidiEventIndexAtTick(int tick);
        // MidiNote  _getActiveNoteAtTick(int tick);
        void _activateMidiEvent(int midieventindex);

        public:

            MonophonicMidiFileReader( 
                std::string filePath, 
                std::shared_ptr<spdlog::logger> logger, 
                float tempobpm, 
                float dts,
                short lengthbars );

            void        resetToTicks(int tick);
            MidiNote    getNextActiveNote();
    };
}
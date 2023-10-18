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

    // int getLengthOfMidiSequence( const std::vector<MidiEventWrapper> &midi_vector );

    class MidiFileWrapper {

        std::string _file_path;
        
        std::vector<MidiEventWrapper> _midi_events;
        std::shared_ptr<spdlog::logger> _logger;
        
        double _duration_s = 0;
        double _tick_duration_s;

        int _total_ticks;
        double _ticks_per_quarter_note;
        double _tempo_bpm;

        // stuff to help reading a file
        float _dt_s;
        float _t_s;
        int _current_tick = 0;
        std::vector<MidiNote> _active_notes_vec;
        int _curr_midi_evt_index = -1;

        void _setInitialNotes(); 

        public:
            // When creating the MFWrapper we want to parse and
            // store the entire track in a single instance 
            // and get rid of the file itself 
            MidiFileWrapper( const std::string &filePath );
            ~MidiFileWrapper();

            void printMidiEvents();
            int getTempoBpm(){ return _tempo_bpm; }
            double getDuration(){ return _duration_s; }
            
            // Called by the Synthple run() to get what notes are 
            // active at a certain time
            void initSequentialRead( float dt_s );
            void step();
            const std::vector<MidiNote> &getActiveNotesVec(){return _active_notes_vec;};
            std::string toString();
    };
}
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
        Note note;
        ushort note_value;
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
            Note _intToNote(int noteNumber);
    };

    // int getLengthOfMidiSequence( const std::vector<MidiEventWrapper> &midi_vector );

    class MidiFileWrapper {

        std::string _file_path;
        
        std::vector<MidiEventWrapper> _midi_events;
        std::shared_ptr<spdlog::logger> _logger;
        
        double _duration_ms = 0;
        double _ticks_per_quarter_note;
        double _tick_duration;
        double _tempo_bpm;

        public:
            // When creating the MFWrapper we want to parse and
            // store the entire track in a single instance 
            // and get rid of the file itself 
            MidiFileWrapper( const std::string &filePath );
            ~MidiFileWrapper();

            void printMidiEvents();
            // void getEventAt(int ms);
            int getTempoBpm(){ return _tempo_bpm; }
            
            // Called by the Synthple run() to get what notes are 
            // active at a certain time
            std::vector<MidiNote> getNoteAtInstant( int ms );
            std::string toString();
    };

    class Loop {

       MidiFileWrapper _contents;
       ushort _length_beats, _beats_per_min;
       uint _length_milliseconds;

       public:

        

    };
}
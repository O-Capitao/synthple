#include <spdlog/sinks/basic_file_sink.h>
#include <midi/synthple_midi.hpp>

using namespace synthple::midi;

/*************************************************************************
 * MIDI NOTE
*************************************************************************/
std::string MidiNote::toString(){
    std::string retval = "";

    switch (note)
    {
        case NoteKey::C:
            retval += "C";
            break;
        case NoteKey::Cs:
            retval += "C#";
            break;
        case NoteKey::D:
            retval += "D";
            break;
        case NoteKey::Ds:
            retval += "D#";
            break;
        case NoteKey::E:
            retval += "E";
            break;
        case NoteKey::F:
            retval += "F";
            break;
        case NoteKey::Fs:
            retval += "F#";
            break;
        case NoteKey::G:
            retval += "G";
            break;
        case NoteKey::Gs:
            retval += "G#";
            break;
        case NoteKey::A:
            retval += "A";
            break;
        case NoteKey::As:
            retval += "A#";
            break;
        case NoteKey::B:
            retval += "B";
            break;
        default:
            throw std::runtime_error("Invalid Note detected.");
    }

    retval += std::to_string(octave);

    return retval;
}

/*************************************************************************
 *  MIDI EVENT WRAPPER Class
*************************************************************************/
MidiEventWrapper::MidiEventWrapper( smf::MidiEvent* mev ){
    
    ticks = mev->tick;
    type = _getMidiEventTypeFromCode( (int)(*mev)[0] );

    // continue parsing only for "allowed" events
    if (type == MidiEventType::NOTE_OFF || type == MidiEventType::NOTE_ON ){
        note = _getMidiEventNoteFromCode( (int)(*mev)[1] );
    }
}

MidiEventWrapper::MidiEventWrapper( MidiEventType _t, MidiNote _n, int _ticks )
:type( _t ), note( _n ), ticks( _ticks )
{}

MidiEventType MidiEventWrapper::_getMidiEventTypeFromCode( int eventCode ){
    switch (eventCode) {
        case 128:
            return MidiEventType::NOTE_OFF;
        case 144:
            return MidiEventType::NOTE_ON;
        case 255:
            return MidiEventType::END_OF_SEQUENCE;
        default:
            return MidiEventType::OTHER;
    }
}

MidiNote MidiEventWrapper::_getMidiEventNoteFromCode( int noteCode ){
    // return {
    //     .note = _intToNote( noteCode % DIATONIC_SCALE_SIZE),
    //     .octave = (ushort)(noteCode / DIATONIC_SCALE_SIZE)
    // };
    return MidiNote( _intToNote( noteCode % DIATONIC_SCALE_SIZE),  (ushort)(noteCode / DIATONIC_SCALE_SIZE) );
}

synthple::NoteKey MidiEventWrapper::_intToNote(int noteNumber){
    switch (noteNumber) {
        case 0:
            return NoteKey::C;
        case 1:
            return NoteKey::Cs;
        case 2:
            return NoteKey::D;
        case 3:
            return NoteKey::Ds;
        case 4:
            return NoteKey::E;
        case 5:
            return NoteKey::F;
        case 6:
            return NoteKey::Fs;
        case 7:
            return NoteKey::G;
        case 8:
            return NoteKey::Gs;
        case 9:
            return NoteKey::A;
        case 10:
            return NoteKey::As;
        case 11:
            return NoteKey::B;
        default:
            throw std::runtime_error("Invalid Note detected, aborting.\n"); 
    }
}

std::string MidiEventWrapper::toString(){
    std::string retval = "\nEvent @:" + std::to_string(ticks) + "\n";

    switch (type){
        case MidiEventType::NOTE_ON:
            retval += "  ON ";
            break;
        case MidiEventType::NOTE_OFF:
            retval += "  OFF ";
            break;
        case MidiEventType::END_OF_SEQUENCE:
            return retval + " END OF SEQUECE";
        default:
            return retval + "  OTHER";
    }

    return retval + note.toString();
}


/*************************************************************************
 *  MIDI FILE WRAPPER Class
*************************************************************************/
MidiFileWrapper::MidiFileWrapper(
    const std::string &fp 
):_logger(spdlog::basic_logger_mt("MIDI", "synthple.log"))
{                                       
    _logger->info("ENTER constructor");

    smf::MidiFile midifile( fp );

    midifile.joinTracks();
    
    _ticks_per_quarter_note = midifile.getTicksPerQuarterNote();
    _tick_duration = midifile.getTimeInSeconds(1);
    
    smf::MidiEvent* mev;

    for (int event=0; event < midifile[0].size(); event++) {

        mev = &midifile[0][event];

        _midi_events.push_back({
            MidiEventWrapper(mev)
        });
    }

    // get track duration from
    // last event's ticks
    long _last_event_ticks = _midi_events.back().ticks;
    _duration_ms = _last_event_ticks * _tick_duration;
    _tempo_bpm = 60 / (_tick_duration * _ticks_per_quarter_note);

    _logger->info("Midi File: \n{}", toString());

    _logger->info("EXIT constructor");
    _logger->flush();
}

MidiFileWrapper::~MidiFileWrapper(){}

// for next step
std::vector<MidiNote> MidiFileWrapper::getNotesAtInstant( int ms )
{   
    std::vector<MidiNote> out;
    int curr_tick = floor( (double)ms / _tick_duration );
    
    for (int i = 0; i < _midi_events.size(); i++){    
        MidiEventWrapper &ev = _midi_events[i];
        if (ev.type == MidiEventType::NOTE_ON && ev.ticks <= curr_tick ){
            // note was preseed
            for (int j = i; j < _midi_events.size(); j++){
                MidiEventWrapper &inner_ev = _midi_events[i];
                if ( inner_ev.note.note_value == ev.note.note_value 
                    && inner_ev.type == MidiEventType::NOTE_OFF )
                {
                    // detect the 1st note off for that note, if it already occured,
                    // ignore, otherwise, play it.
                    if (inner_ev.ticks > curr_tick){
                        out.push_back( ev.note );
                    }
                }
            }
        }
    }
    return out;
}

MidiNote MidiFileWrapper::getSingleNoteAtInstant( int ms )
{   
    MidiNote out;
    int curr_tick = floor( (double)ms / _tick_duration );
    
    for (int i = 0; i < _midi_events.size(); i++){  

        MidiEventWrapper &ev = _midi_events[i];
        if (ev.type == MidiEventType::NOTE_ON && ev.ticks <= curr_tick ){
            // note was preseed
            for (int j = i; j < _midi_events.size(); j++){
                MidiEventWrapper &inner_ev = _midi_events[i];
                if ( inner_ev.note.note_value == ev.note.note_value && inner_ev.type == MidiEventType::NOTE_OFF )
                {
                    out = ev.note;
                    break;
                }
            }
        }
    }
    return out;
}

std::string MidiFileWrapper::toString(){
    
    std::string retval;

    retval.append(
    "Parsed MIDI file:\n  total duration = "
     + std::to_string(_duration_ms) 
     + "s\nticks_per_qnote ="
     + std::to_string(_ticks_per_quarter_note)
     + "\nbpm: "
     + std::to_string( _tempo_bpm )
     + "\ntick duration: "
     + std::to_string( _tick_duration ) 
     + "\nEvents:\n" 
    );
    
    for (MidiEventWrapper &ev : _midi_events){
        retval.append(ev.toString());
        retval.append("\n");
    }

    return retval;
}
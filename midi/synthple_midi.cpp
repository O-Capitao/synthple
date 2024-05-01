#include <spdlog/sinks/basic_file_sink.h>
#include <midi/synthple_midi.hpp>

using namespace synthple::midi;

// MidiNote ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
std::string MidiNote::toString(){
    std::string retval = "";

    switch (note)
    {
        case NoteKey::C:
            retval += "C";
            break;
        case NoteKey::Cs:
            retval += "Cs";
            break;
        case NoteKey::D:
            retval += "D";
            break;
        case NoteKey::Ds:
            retval += "Ds";
            break;
        case NoteKey::E:
            retval += "E";
            break;
        case NoteKey::F:
            retval += "F";
            break;
        case NoteKey::Fs:
            retval += "Fs";
            break;
        case NoteKey::G:
            retval += "G";
            break;
        case NoteKey::Gs:
            retval += "Gs";
            break;
        case NoteKey::A:
            retval += "A";
            break;
        case NoteKey::As:
            retval += "As";
            break;
        case NoteKey::B:
            retval += "B";
            break;
        case NoteKey::NOT_A_NOTE:
            retval += "NOT_A_NOTE";
            break;
        default:
            throw std::runtime_error("Invalid Note detected.");
    }

    retval += std::to_string(octave);

    return retval;
}


// MidiEventWrapper////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MidiEventWrapper::MidiEventWrapper( smf::MidiEvent* mev ){
    
    ticks = mev->tick;
    type = _getMidiEventTypeFromCode( (int)(*mev)[0] );

    // continue parsing only for "allowed" events
    if (type == MidiEventType::NOTE_OFF || type == MidiEventType::NOTE_ON ){
        note = _getMidiEventNoteFromCode( (int)(*mev)[1] );
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MidiEventWrapper::MidiEventWrapper( MidiEventType _t, MidiNote _n, int _ticks )
:type( _t ), note( _n ), ticks( _ticks )
{}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MidiNote MidiEventWrapper::_getMidiEventNoteFromCode( int noteCode ){
    // return {
    //     .note = _intToNote( noteCode % DIATONIC_SCALE_SIZE),
    //     .octave = (ushort)(noteCode / DIATONIC_SCALE_SIZE)
    // };
    return MidiNote( _intToNote( noteCode % DIATONIC_SCALE_SIZE),  (ushort)(noteCode / DIATONIC_SCALE_SIZE) );
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
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


// MidiFileWrapper/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MidiFileWrapper::MidiFileWrapper(
    std::string fp, std::shared_ptr<spdlog::logger> logger
){
    _logger = logger;                                     
    _logger->info("MIDI: ENTER constructor");

    smf::MidiFile midifile( fp );

    midifile.joinTracks();

    assert(midifile.getEventCount(0) > 0);
    
    _ticks_per_quarter_note = midifile.getTicksPerQuarterNote();
    _tick_duration_s = midifile.getTimeInSeconds(1);
    
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
    _duration_s = _last_event_ticks * _tick_duration_s;
    _tempo_bpm = 60 / (_tick_duration_s * _ticks_per_quarter_note);

    

    _logger->info("MIDI: Midi File: \n{}", toString());

    _logger->info("MIDI: EXIT constructor");
    _logger->flush();
}

// MidiFileWrapper::~MidiFileWrapper(){}

void MidiFileWrapper::initSequentialRead( float dt_s ){
    
    _dt_s = dt_s;
    _t_s = 0;

    _current_tick = 0;

    _active_notes_vec = std::vector<MidiNote>(MAX_N_VOICES);
    
    _active_notes_vec[0] = MidiNote(NoteKey::NOT_A_NOTE, 0);
    _active_notes_vec[1] = MidiNote(NoteKey::NOT_A_NOTE, 0);
    _active_notes_vec[2] = MidiNote(NoteKey::NOT_A_NOTE, 0);
    _active_notes_vec[3] = MidiNote(NoteKey::NOT_A_NOTE, 0);

    _curr_midi_evt_index = 0;

    _setInitialNotes();
}

void MidiFileWrapper::_setInitialNotes(){

    if (_midi_events[0].ticks == 0 && _midi_events[0].type == MidiEventType::NOTE_ON){
        _active_notes_vec[0] = _midi_events[0].note;
        _curr_midi_evt_index = 0;

    }

}

void MidiFileWrapper::step(){

    if (_curr_midi_evt_index > _midi_events.size() - 1) return;
    
    MidiEventWrapper &next_event = _midi_events[ _curr_midi_evt_index + 1 ];

    if (next_event.type == MidiEventType::END_OF_SEQUENCE) return;

    if (next_event.ticks <= _current_tick){
        _logger->debug("MIDI: Changing Midi state");
        _logger->flush();
        if (next_event.type == MidiEventType::NOTE_ON){
            _active_notes_vec[0] = next_event.note;
        } else 
        if (next_event.type == MidiEventType::NOTE_OFF && _active_notes_vec[0].note_value == next_event.note.note_value){
            _active_notes_vec[0] = MidiNote(NoteKey::NOT_A_NOTE, 0);
        }

        _curr_midi_evt_index++;
    }

    _t_s += _dt_s;
    _current_tick = _t_s / _tick_duration_s;
}

std::string MidiFileWrapper::toString(){
    
    std::string retval;

    retval.append(
    "Parsed MIDI file:\n  total duration = "
     + std::to_string(_duration_s) 
     + "s\nticks_per_qnote ="
     + std::to_string(_ticks_per_quarter_note)
     + "\nbpm: "
     + std::to_string( _tempo_bpm )
     + "\ntick duration: "
     + std::to_string( _tick_duration_s ) 
     + "\nEvents:\n" 
    );
    
    for (MidiEventWrapper &ev : _midi_events){
        retval.append(ev.toString());
        // retval.append("         ,        ");
    }

    return retval;
}



// MonophonicMidiFileReader ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MonophonicMidiFileReader::MonophonicMidiFileReader(
    std::string fp,
    std::shared_ptr<spdlog::logger> logger,
    float tempobpm,
    float dts
){
    _logger = logger;                                     
    _logger->info("MIDI: ENTER constructor");

    smf::MidiFile midifile( fp );
    midifile.joinTracks();

    _populateMidiEvents();



}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void MonophonicMidiFileReader::reset(){

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MidiNote MonophonicMidiFileReader::getNextActiveNote(){

}

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


// MonophonicMidiFileReader ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MonophonicMidiFileReader::MonophonicMidiFileReader(
    std::string fp,
    std::shared_ptr<spdlog::logger> logger,
    float tempobpm,
    float dts,
    short lbars
):_tempo_bpm(tempobpm),
_dt_s(dts),
_length_bars(lbars)
{
    _logger = logger;                                     
    _logger->info("MIDI: path = \"{}\", ENTER constructor", fp);

    smf::MidiFile midifile( fp );
    midifile.joinTracks();

    assert(midifile.getEventCount(0) > 0);

    // file's own tempo maybe different from actual target tempo.
    _ticks_per_beat = midifile.getTicksPerQuarterNote();
    
    float _file__tick_duration_s = midifile.getTimeInSeconds(1);
    float _file__tempo_bpm = (60 * _ticks_per_beat) / _file__tick_duration_s;

    _tick_duration_s = _tempo_bpm / (60 * _ticks_per_beat);

    // TODO : time signatures?
    _ticks_per_loop = 4 * _ticks_per_beat;

    _populateMidiEvents(midifile);
    resetToTicks(0);

    _logger->debug(
        "MIDI: path = \"{}\", exiting constructor. \"_tick_duration_s\"={} s, \"_ticks_per_beat\"={}.",
        fp, _tick_duration_s, _ticks_per_beat );
    _logger->flush();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void MonophonicMidiFileReader::_populateMidiEvents(smf::MidiFile &file){
    
    smf::MidiEvent* mev;

    for (int event=0; event < file[0].size(); event++) {

        mev = &file[0][event];

        _midi_events.push_back({
            MidiEventWrapper(mev)
        });
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int MonophonicMidiFileReader::_getMidiEventIndexAtTick(int tick){
    int retval = 0;
    for (MidiEventWrapper &_mew : _midi_events){
        if (_mew.ticks < tick ){
            retval ++;
        } else {
            break;
        }
    }
    return retval;
}

// ///////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////
// MidiNote MonophonicMidiFileReader::_getActiveNoteAtTick(int tick){
//     MidiNote retval = _midi_events[]
// }

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void MonophonicMidiFileReader::_activateMidiEvent(int midieventindex){
    MidiEventWrapper &mew = _midi_events[midieventindex];
    _active_note = mew.note;

    if (mew.type == MidiEventType::NOTE_OFF){
        _is_silent = true;
    } else {
        _is_silent = false;
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void MonophonicMidiFileReader::resetToTicks(int tick){
    _current_tick = tick;
    _current_time_s = _tick_duration_s * tick;
    _current_midi_evt_index = _getMidiEventIndexAtTick(tick);
    _activateMidiEvent(_current_midi_evt_index);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
MidiNote MonophonicMidiFileReader::getNextActiveNote(){
    
     // check current time:
    MidiEventWrapper &next_event = _midi_events[ _current_midi_evt_index++ ];

    if (next_event.ticks <= _current_tick){
        // advance event
        if (next_event.type == MidiEventType::NOTE_ON){
            _active_note = next_event.note;
        } else if (next_event.type == MidiEventType::NOTE_OFF && _active_note.note_value == next_event.note.note_value){
            _active_note = MidiNote(NoteKey::NOT_A_NOTE, 0);
        }
        _active_note = next_event.note;
    }

    // increment time
    _current_time_s += _dt_s;
    _current_tick = (int)floor(_current_time_s / _tick_duration_s);

    // check bounds and loop
    int _ticks_over = _current_tick - _ticks_per_loop;

    if (_ticks_over > 0){
        // reset to ticks
        resetToTicks(_ticks_over);
    }

    return _active_note;
}

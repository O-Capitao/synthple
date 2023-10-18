#include <synthple.hpp>
#include <synthple_audio.hpp>

#include <math.h>
#include <time.h> 

#include <spdlog/sinks/basic_file_sink.h>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

using namespace synthple;

//
// Generators
Generator::Generator( float amp, float freq_hz )
:
_logger(spdlog::basic_logger_mt("Generator", "synthple.log"))
{
    _logger->set_level(spdlog::level::debug);
    setNewAmpAndFreq( amp, freq_hz );
}

Generator::~Generator()
{
    _logger->flush();
}

void Generator::setNewAmpAndFreq(float a, float f)
{
    _amp = a;
    _freq_hz = f;
    _freq_rad_s = _freq_hz * (2 * M_PI);
    _period = 1.0f / _freq_hz;

    
    _logger->debug("Generator Freq Changed: _period is {} - _freq_rad_s is {}",
        _period, _freq_rad_s);
    _logger->flush();
}

//
// Sine Generator
SineGenerator::SineGenerator(float amp, float freq_hz )
:Generator(amp,freq_hz)
{
    _logger->info("Generator is Sine Generator.");
}

SineGenerator::~SineGenerator()
{}

float SineGenerator::getValueAtTime( float t )
{
    float retval = _amp * sin( _freq_rad_s * t );

    return retval;
}

// //
// // Square Generator
// SquareGenerator::SquareGenerator(float amp, float freq_hz )
// :Generator(amp,freq_hz)
// {
//     _logger->info("Generator is Square Generator.");
// }

// SquareGenerator::~SquareGenerator()
// {}

// float SquareGenerator::getValueAtTime( float t )
// {
//     float __t_in_period = t - floor( t / _period );

//     return 0.0f;
// }


Synthple::Synthple( bus::AudioDataBus *audioDataBus, midi::MidiFileWrapper *midifile )
:
_logger(spdlog::basic_logger_mt("SYNTHPLE", "synthple.log")),
_audioThread( audio::AudioThread( audioDataBus ) ),
_audioDataBus_ptr( audioDataBus ),
_midi_file_ptr( midifile ),
_bpm(midifile->getTempoBpm()),
_isNotePressed(false),
_noteFrequency(NoteFrequency()),
_pressedNote( midi::MidiNote( NoteKey::A, 3 )),
_totalTime_s( midifile->getDuration() ),
// _generator( 0.25, _noteFrequency.findFrequecyForNote( _pressedNote ) )
_generator(0.25, _noteFrequency.noteFreqMap[_pressedNote.note_value])
{
    _logger->set_level(spdlog::level::debug);
    _logger->debug("Constructed Synthple obj");
}

Synthple::~Synthple()
{}

void Synthple::run()
{   
    
    // debug stuff
    clock_t _ticks_start = clock();
    int _writes_to_buffer = 0;
    int _predicted_sample_count = _totalTime_s * FRAMERATE;
    int _buffer_writes = _predicted_sample_count / FRAMES_IN_BUFFER;

    _logger->info(
        "Starting run. Predicted sample count to be handled: {}", 
        _predicted_sample_count);

    _logger->debug(
        "Size of buffer: {}. - Writes to Buffer (Sample Count / Buffer Size) = {}", 
        FRAMES_IN_BUFFER, _buffer_writes
    );

    // should be the same as the target sample rate
    float _logical_dt_s = 1 / (float)FRAMERATE;

    float _logical_current_time_s = 0;
    float _total_time_s = _totalTime_s;

    float _cycle_advance_dt_s = 00.5 * (float)FRAMES_IN_BUFFER / (float)FRAMERATE;
    float _this_frames_val = 0;

    std::string _current_note_name = "";
    std::vector<NoteKey> activenotes(MAX_N_VOICES);

    _logger->debug("Starting Run - Cycle Time = {}", _cycle_advance_dt_s);
    
    _midi_file_ptr->initSequentialRead( _logical_dt_s );
    const std::vector<midi::MidiNote> &_requestedNotes = _midi_file_ptr->getActiveNotesVec();

    // quick and dirty test,
    // use only 1st note of current chord
    midi::MidiNote _requestedNote;
    float _requestedNoteFreq = 0;

    _audioThread.start();

    while (!_MAIN_QUIT)
    {
        // PROCESS AUDIO
        int _spaceInQueue = _audioDataBus_ptr->queue.write_available();

        spdlog::debug("Starting Buffer Write. '_logical_current_time_s'={}", _logical_current_time_s);

        if ( _spaceInQueue > 0 && _logical_current_time_s < _total_time_s )
        {
            // advance time as we push values into the queue
            for (int i = 0; i < _spaceInQueue; i++ )
            {
                // TIME STEP
                if (_logical_current_time_s >= _total_time_s) {
                    break;
                    _MAIN_QUIT = true;
                }

                // PROCESS MIDI
                _requestedNote = _requestedNotes[0];
                
                
                // DEBUG
                if (_requestedNote.note_value != _current_note_name ){
                    _requestedNoteFreq = _requestedNote.note == NoteKey::NOT_A_NOTE ? 0 : _noteFrequency.noteFreqMap[ _requestedNote.note_value ];
                    _logger->debug("***Note Changed*** To: {} at {} s. Freq={} Hz",
                        _requestedNote.note_value, _logical_current_time_s, _requestedNoteFreq);
                    _current_note_name = _requestedNote.note_value;
                }

                if (_requestedNote.note == NoteKey::NOT_A_NOTE )
                {
                    _isNotePressed = false;
                } else {
                
                    if (_pressedNote.note_value != _requestedNote.note_value){

                        _generator.setNewAmpAndFreq( 0.25, _noteFrequency.noteFreqMap[ _requestedNote.note_value ] );
                    }
                    _pressedNote = _requestedNote;
                    _isNotePressed = true;
                }
                

                _this_frames_val = _isNotePressed ? _generator.getValueAtTime( _logical_current_time_s ) : 0.0f;

                _audioDataBus_ptr->queue.push( _this_frames_val );
                // _audioDataBus_ptr->queue.push( 0 );
                _logical_current_time_s += _logical_dt_s;
                _midi_file_ptr->step();
                // END  TIME STEP
            }

            _writes_to_buffer++;
            
        }

        // let it run until buffer is depleted
        if (_logical_current_time_s >= _total_time_s && _audioDataBus_ptr->queue.empty()){
            _MAIN_QUIT = true;
        }
        // _logger->debug("tick: _logical_current_time_s: {}", _logical_current_time_s);
        boost::this_thread::sleep_for(boost::chrono::milliseconds((int)floor(_cycle_advance_dt_s * 1000)));
    }

    _audioThread.stop();

    // Time at end of run
    clock_t _length_ticks = clock() - _ticks_start;
    float _length_secs = (float)_length_ticks/ (float)CLOCKS_PER_SEC;

    _logger->info("run() Finished - took {} s and wrote {} times to the buffer" , 
        _length_secs, 
        _writes_to_buffer);
    
}

void Synthple::quit()
{
    _MAIN_QUIT = true;
}
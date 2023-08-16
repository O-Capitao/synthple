#include <synthple.hpp>
#include <synthple_audio.hpp>

#include <math.h>
#include <spdlog/sinks/basic_file_sink.h>
// #include <boost/timer/timer.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

// using boost::timer::cpu_timer;
// using boost::timer::cpu_times;
// using boost::timer::nanosecond_type;

using namespace synthple;


/**
 * Simplest Clock
*/
Synthple_Clock::Synthple_Clock(){
    _ticks_start = clock();
}

void Synthple_Clock::resetClock(){
    _ticks_start = clock();
}

unsigned long Synthple_Clock::getTicks(){
    return clock() - _ticks_start;
}






/***
 * Oscilator
*/
float Oscillator::produceVal(){
    return sin( _amp * _t * _freq );
}

void Oscillator::reset(){
    _amp = 0;
    _t = 0;
    _freq = 0;
}

void Oscillator::setFrequency( float newFreq  ){
    _t = 0;
    _freq = newFreq;
}

void Oscillator::setAmp( float newAmp ){

    assert( newAmp < 1 && newAmp > -1);
    _amp = newAmp;
}





/*
*   Synthple
*/
Synthple::Synthple( 
    midi::MidiFileWrapper &midiFile,
    bus::AudioData_Queue *sample_queue_ptr,
    bus::Commands_Queue *cmd_queue_ptr
):_logger(spdlog::basic_logger_mt("SYNTHPLE", "synthple.log")),
_midiFile(midiFile)
{
    _logger->set_level(spdlog::level::trace);
    

    _sample_queue_ptr = sample_queue_ptr;
    _cmd_queue_ptr = cmd_queue_ptr;

    // TODO - config via file
    _n_oscilators = 1;
    Oscillator _osc;
    _osc.setAmp( 0.5 );

    _oscilators.push_back( _osc );   

    _logger->debug("Constructed Synthple obj.\n     Buffer duration: {}", _buffer_duration);
    _logger->flush();
}

Synthple::~Synthple()
{
    _logger->debug("~Synthple");
    _logger->flush();
}

void Synthple::run()
{

    _logger->debug("run(), Starting.");
    _logger->flush();

    double time_counter = 0;
    float frame_value = 0;
    bool carry_last_computed_frame = false;

    synthple::audio::AudioThread _audio_thread(
        _sample_queue_ptr,
        _cmd_queue_ptr
    );
    
    boost::thread playT{
        boost::bind(
            &synthple::audio::AudioThread::run, &_audio_thread 
        )
    };

    // _audio_thread.run();

    _logger->debug("run(), Spawned audio thread.");
    _logger->flush();

    _playing = true;    

    // boost::timer::cpu_timer __timer;
    // boost::timer::cpu_times __ellapsed, __last;
    // boost::timer::nanosecond_type const __step_time( 0.03 * 1000000000LL );
    // __last = __timer.elapsed();

    
    boost::chrono::steady_clock _clock;
    boost::chrono::time_point _now = _clock.now();
    boost::chrono::time_point _then(_now);
    boost::chrono::duration


    _logger->debug("run(): inited _clock");
    while (_playing){

        // __ellapsed = __timer.elapsed();

        // if (__ellapsed - __last >= __step_time ){

        // }


        // the main thread will run every 
        // dt to process midi events and produce audio data

        
        if (!carry_last_computed_frame){

            frame_value = 0;

            for (Oscillator osc : _oscilators){
                frame_value += osc.produceVal();
            }
        }


        // // handle events
        // Signal _signal;
        // while (_cmd_queue_ptr->queue.pop( _signal )){
        //     if (_signal == Signal::PLAYBACK_FINISHED){
                
        //         _logger->debug("PLAYBACK_FINISHED signal received in main run() loop.");
        //         _logger->flush();

        //         _playing = false;
        //         break;
        //     }
        // }

        int available_size_in_queue = _sample_queue_ptr->queue.read_available();

        // push new values into buffer if space is available;
        if (available_size_in_queue == BUFFER_SIZE){

            _sample_queue_ptr->queue.push(frame_value);
            time_counter += _dt_s;
        
        } else {

            // sleep for a while let the other thread work
            boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
        }

        
    }

    _logger->debug("Exiting run()");
    _logger->flush();
}
#include <synthple.hpp>
#include <synthple_audio.hpp>

#include <math.h>
#include <time.h> 

#include <spdlog/sinks/basic_file_sink.h>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

using namespace synthple;

//
// Generator
Generator::Generator( float amp, float freq_hz )
:_amp(amp),_freq_hz(freq_hz)
{
    _period = 1 / _freq_hz;
    _freq_rad_s = _freq_hz / (M_2_PI);
}

Generator::~Generator()
{}

float Generator::getValueAtTime( float t )
{
    float __t_in_period = t - floor( t / _period );
    return _amp * sin( _freq_rad_s * __t_in_period );
}





//
// Synthple
Synthple::Synthple( bus::AudioDataBus *audioDataBus )
:_audioThread( audio::AudioThread( audioDataBus ) ),
_audioDataBus_ptr( audioDataBus ),
_generator( 0.5, 440.0 ),
_totalTime_s(3)
{}

Synthple::~Synthple()
{}

void Synthple::run()
{
  
    float _logical_dt_s = 0.01;
    float _logical_current_time_s = 0;
    float _total_time_s = _totalTime_s;

    while (!_MAIN_QUIT)
    {
        int _spaceInQueue = _audioDataBus_ptr->queue.write_available();

        if ( _spaceInQueue > 0 && _logical_current_time_s < _total_time_s )
        {
            // advance time as we push values into the queue
            for (int i = 0; i < _spaceInQueue; i++ )
            {
                _audioDataBus_ptr->queue.push( _generator.getValueAtTime( _logical_current_time_s ) );
                _logical_current_time_s += _logical_dt_s;
            }
        }

        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
}

void Synthple::quit()
{
    _MAIN_QUIT = true;
}
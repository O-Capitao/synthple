#include <synthple_globals.hpp>
#include <synthple_waves.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <math.h>

using namespace synthple::waves;

Wave::Wave(float freqHz)
{
    _freq_hz = freqHz;
    _freq_rad_s = _freq_hz * (2 * M_PI);
}

SineWave::SineWave(float freqHz)
:Wave(freqHz)
{}

float SineWave::getValue( float t ){
    return cos(_freq_rad_s  * t );
}




WaveGenerator::WaveGenerator( float g ):
_logger(spdlog::basic_logger_mt("GENERATORS", "synthple.log")),
_wave(SineWave(440)),
_gain(g)
{
    _logger->set_level(spdlog::level::debug);
}

void WaveGenerator::step( float dt ){
    
    if (_is_transient){

        // manage fades
        if (_is_fadingIn){

            if (_fade_factor < 1){

                _fade_factor += _fade_increment_val;
                _value = _fade_factor * _gain * _wave.getValue(_int_t);

            } else {
                // disarm fade
                _is_fadingIn = false;
                _is_transient = false;
                goto steady_state;
            }

        } else if (_is_fadingOut){

            if (_fade_factor > 0){

                _fade_factor -= _fade_increment_val;
                _value = _fade_factor * _gain * _wave.getValue(_int_t);

            } else {
                // disarm fade
                _is_fadingIn = false;
                _is_transient = false;
                _is_silent = true;

                goto steady_state;
            }

        } else {

            float aux_val = 0;

            // wave to wave transition
            aux_val = _wave.getValue(_int_t);

            if (aux_val <= _change_amp_thresh){
                _value = _gain * aux_val;
            } else {

                _logger->debug("WAVE 2 WAVE - threshold reached!");
                _logger->debug("last: {}", aux_val*_gain);

                // disarm transient
                _is_transient = false;

                _wave = _new_wave;
                _int_t = 0;
                _value = _gain * _wave.getValue(_int_t);

                _logger->debug("into: {}", _value);
            }
        }


    } else {
        steady_state:
        _value = _is_silent ? 
            0 : 
            _gain * _wave.getValue(_int_t);
    }
    
    _int_t += dt;
    assert( _value > -1 && _value < 1 );
}

void WaveGenerator::requestFreqChange( float newFreqHz )
{
    _new_wave = SineWave( newFreqHz );
    
    // set relevant flags
    _is_transient = true;

    if (_is_silent){

        _wave = SineWave( newFreqHz );
        _new_wave = NULL;

        // reset internal counter : waves start at t = 0;
        _int_t = 0 ;

        // transitioning from silence
        _is_fadingIn = true;
        _is_fadingOut = false;
        _is_silent = false;
        _fade_factor = 0.0;
        _t_fade = 0.0;
    } else {
        // wave into wave transient
        _is_fadingIn = false;
        _is_fadingOut = false;
    }
}

void WaveGenerator::requestSilence()
{
    if (_is_silent) return;

    if (_is_transient && ( _is_fadingIn || _is_fadingOut ) )
    {
        // silence requested when transient is already engaged  
        // just switch fade direction if needed and continue
        _is_fadingOut = true;
        _is_fadingIn = false;

    } else {

        _new_wave = NULL;
        _is_transient = true;
        
        // transitioning into silence
        _is_fadingIn = false;
        _is_fadingOut = true;
        _is_silent = false;
        _fade_factor = 0.0;
        _t_fade = 0.0;
    }
}

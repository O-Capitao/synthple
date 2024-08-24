#include <synthple_oscillator.hpp>

#include <math.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace synthple::oscillator;

// OSCILLATORS:               /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void BaseOscillator::requestSilence()
{
    _isSilenceRequested = true;
}


// SquareOscillator:          /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
SquareOscillator::SquareOscillator( float inflx )
:_inflection(inflx)
{}

float SquareOscillator::getValueAt(float t_s)
{

    if ( fmod(t_s, _outputPeriod) <= _inflection_in_cycle ) {
        return 0.0f;
    } else {
        return 1.0f;
    }
}

void SquareOscillator::setFrequency( float newfreq )
{
    _outputFreq = newfreq;
    _outputPeriod = 1 / newfreq;
    _outputSilence = false;
    _isSilenceRequested = false;
    _inflection_in_cycle = _inflection  * _outputPeriod;
}


// TriangleOscillator:          ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////
TriangleOscillator::TriangleOscillator( float inflx )
:_inflection(inflx){}

void TriangleOscillator::setFrequency( float newfreq )
{
    _outputFreq = newfreq;
    _outputPeriod = 1 / newfreq;
    _outputSilence = false;
    _isSilenceRequested = false;

    _slope_asc = 1 / (_inflection * _outputPeriod);
    _slope_desc = 1 / _outputPeriod * ( 1 - _inflection );

}

float TriangleOscillator::getValueAt(float t_s)
{
    
    float _t_aux = fmod(t_s, _outputPeriod);
    float _offset_in_cycle = _inflection  * _outputPeriod;

    
    if ( _t_aux <= _offset_in_cycle ) {
        return _t_aux * _slope_asc;
    } else {
        return 1 - _t_aux * _slope_desc;
    }

}


// SinOscillator:               ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////
SinOscillator::SinOscillator( int n_samples )
:_n_samples(n_samples),
_wavetable(n_samples)
{
    // init wt data.
    float angle_step = (2 * M_PI) / (float)_n_samples;

    for (int i = 0; i < _n_samples; i++){
        _wavetable[i] = sin( i * angle_step );
    }
}

void SinOscillator::setFrequency( float newfreq )
{
    _outputFreq = newfreq;
    _outputPeriod = 1 / newfreq;
    _outputSilence = false;
    _isSilenceRequested = false;
}

float SinOscillator::getValueAt(float t_s)
{
    
    float _t_aux = fmod(t_s, _outputPeriod);
    int _sample_index = (int)floor((_t_aux / _outputPeriod ) * (float)_n_samples);

    assert(_sample_index < _n_samples && _sample_index >= 0);

    return _wavetable[ _sample_index ];
}
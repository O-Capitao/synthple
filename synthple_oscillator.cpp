#include <synthple_oscillator.hpp>

#include <math.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace synthple::oscillator;


// Wave Table /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
WaveTable::WaveTable( int nsamples, std::string wavetabletype_str, std::shared_ptr<spdlog::logger >logger )
:_logger(logger),
_n_samples(nsamples),
_wt_type( mapStringToWaveTableType(wavetabletype_str) ),
_data( nsamples )
{

    switch( _wt_type ){
        case WaveTableType::SINE:
            _generateSinWaveform();
            break;
        case WaveTableType::SQUARE:
            _generateSquareWaveform();
            break;
        default:
            throw std::runtime_error("WTABLE: invalid WaveTableType.");
    }
    _logger->set_level(spdlog::level::debug);
    _logger->debug("WTABLE: constructed with {} samples", _n_samples);
    _logger->flush();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
float WaveTable::lookupValueAt(short i){
    assert(i < _n_samples);
    return _data[i];
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void WaveTable::_generateSinWaveform(){
    float angle_step = (2 * M_PI) / (float)_n_samples;

    for (int i = 0; i < _n_samples; i++){
        _data[i] = sin( i * angle_step );
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void WaveTable::_generateSquareWaveform(){

    for (int i = 0; i < _n_samples; i++){
        _data[i] = i <= _n_samples / 2 ? 0 : 1;
    }

    // give it some countour
    for (int i = 3; i < _n_samples; i++){    
        _data[i] = (_data[i] + _data[i-1] + sqrtf(_data[i-2]))/3;
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
WaveTableType WaveTable::mapStringToWaveTableType( std::string wtt_str ){
    if (wtt_str == "SINE"){
        return WaveTableType::SINE;
    } else if (wtt_str == "SQUARE" ) {
        return WaveTableType::SQUARE;
    } else if (wtt_str == "SAW" ) {
        return WaveTableType::SAW;
    } else if (wtt_str == "TRIANGLE" ) {
        return WaveTableType::TRIANGLE;
    } else {
        throw std::runtime_error("invalid Wave Table Type.");
    }
}

//
// SquareOscillator
SquareOscillator::SquareOscillator( float inflx )
:_inflection(inflx)
{}

float SquareOscillator::getValueAt(float t_s)
{
    
    float _t_aux = fmod(t_s, _outputPeriod);
    float _inflection_in_cycle = _inflection  * _outputPeriod;

    
    if ( _t_aux <= _inflection_in_cycle ) {
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

}


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

void BaseOscillator::requestSilence()
{
    _isSilenceRequested = true;
}
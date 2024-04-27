#include <synthple_oscillator.hpp>

#include <math.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace synthple::oscillator;


// Wave Table /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
WaveTable::WaveTable( int nsamples, std::string wavetabletype_str )
:_logger(spdlog::basic_logger_mt("WAVETABLE", "synthple.log")),
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
            throw std::runtime_error("invalid WaveTableType.");
    }
    _logger->set_level(spdlog::level::debug);
    _logger->debug("constructed.");
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



//Oscillator //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
Oscillator::Oscillator( std::string wavetabletype_str, int wavetable_nsamples )
:_logger(spdlog::basic_logger_mt("OSCILLATOR", "synthple.log")),
_waveTable(wavetable_nsamples, wavetabletype_str)
{}

float Oscillator::getValueAt(float t_ms){

    float retval = 0;

    if (!_outputSilence){

        float _t_in_cycle = fmod( (t_ms), _outputPeriod );
        short _index = (short)floor(((float)_waveTable.get__n_samples() * _t_in_cycle) / (_outputPeriod));

        assert(_index < _waveTable.get__n_samples());

        if (_isSilenceRequested && (_lastIndex > _index)){
            _isSilenceRequested = false;
            _outputSilence = true;
            
            retval = 0;
        } else {
            
            retval = _waveTable.lookupValueAt( _index );
        }

        _lastIndex = _index;
    }

    return retval;
}

void Oscillator::requestSilence(){
    _isSilenceRequested = true;
}

void Oscillator::setFrequency(float newf){
    _outputFreq = newf;
    _outputPeriod = 1 / newf;
    _outputSilence = false;
    _isSilenceRequested = false;
}

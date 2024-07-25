#include <synthple.hpp>
#include <synthple_audio.hpp>

#include <math.h>
#include <time.h> 

#include <spdlog/sinks/basic_file_sink.h>
// #include <boost/chrono.hpp>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 

using namespace synthple;
using namespace filedata;

// Synthple ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
Synthple::Synthple( std::string path_to_config )
:_logger(spdlog::basic_logger_mt("SYNTHPLE", "synthple.log"))
{
    _logger->set_level(spdlog::level::debug);
    _logger->debug("\n***************************************\n***************************************\n\n\nStarting Synthple.");
    _logger->flush();

    _audioThread.init( &_audioDataBus );
    _filedata.init(path_to_config);

    _logger->debug("Finished Construction.");

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Synthple::init(){
    _audioThread.start();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Synthple::close(){
    _logger->debug("closing");
    _playThread.join();
    _audioThread.stop();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Synthple::setSong( const std::string &song_to_set ){
    
    SongFileData *sfd = _filedata.getSongByName( song_to_set );

    assert(sfd->sections.size() > 0 && sfd->voices.size() > 0);

    // _mixer.loadSong(sfd);
    _activeSong_id = sfd->id;
    _active_song_section_index = 0;

    _mixer.setSong(sfd);
    _playThread = boost::thread( boost::bind(&Synthple::_run, this) );
    _logger->info("Loaded Song with Id = {}", _activeSong_id);

    _logger->flush();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Synthple::setSongSection( int sectionindex ){

    _mixer.setSection(sectionindex);
    _active_song_section_index = sectionindex;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Synthple::_run(){

    bool _EXIT = false;
    int __space_in_queue;
    bus::Command __aux_command;

    // aux data structure
    float __aux_audio_data[BUFFER_SIZE];

    _logger->debug("entering _run() / while loop");

    while (!_EXIT){
        __space_in_queue = _audioDataBus.queue.write_available();

        // process Audio
        if (__space_in_queue > 0){
            _mixer.produceData( __aux_audio_data, __space_in_queue );
            _pushToAudioDataBus( __aux_audio_data, __space_in_queue );
        }

        // process Commands.
        while ( _commandBus.queue.pop(__aux_command)){
            _logger->debug("got a command!");
            
            if (__aux_command.cmdType == bus::CommandType::STOP){
                _EXIT = true;
            }
        }

        boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
    }

    // _close();
    _logger->debug("_run() exiting.");
    
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Synthple::_pushToAudioDataBus( float *topush_data_arr, int topush_length ){
    bool _push_action_retval;
    for (int i = 0; i < topush_length; i++){
        _push_action_retval = _audioDataBus.queue.push( topush_data_arr[i] );
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void Synthple::stop(){
    _logger->debug("stopping.");
    _logger->flush();
    _commandBus.queue.push({
        .cmdType = bus::CommandType::STOP,
        .arg = ""
    });
}

#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <synthple_globals.hpp>
#include <string>


namespace synthple::bus {
    struct AudioDataBus{
        boost::lockfree::spsc_queue<float,boost::lockfree::capacity<BUFFER_SIZE>> queue;
    };

    enum CommandType{
        PLAY,
        PAUSE,
        STOP,
        SET_SONG,
        SET_SECTION
    };

    struct Command {
        CommandType cmdType;
        std::string arg;
    };

    struct CommandBus{
        boost::lockfree::spsc_queue<Command,boost::lockfree::capacity<128>> queue;
    };
}
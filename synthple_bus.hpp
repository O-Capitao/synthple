#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <synthple_globals.hpp>


namespace synthple::bus {
    struct AudioDataBus{
        boost::lockfree::spsc_queue<float,boost::lockfree::capacity<BUFFER_SIZE>> queue;
    };
}
#pragma once

#include <boost/lockfree/spsc_queue.hpp>
#include <synthple_globals.hpp>


namespace synthple::bus {
    
    struct AudioData_Queue {
        boost::lockfree::spsc_queue<uint8_t,boost::lockfree::capacity<BUFFER_SIZE>> queue;
    };

    struct Commands_Queue {
        boost::lockfree::spsc_queue<Signal,boost::lockfree::capacity<10>> queue;
    };
}
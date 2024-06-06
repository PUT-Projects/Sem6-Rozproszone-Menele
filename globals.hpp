#ifndef GLOBALS_HPP
#define GLOBALS_HPP
#include "request_t.hpp"
#include <queue>
#include <mutex>

namespace globals
{
    inline int rank = -1;
    inline int size = -1;
    inline int ack_count = 0;
    inline int lamport_clock = 0;

    inline std::mutex lamport_mutex{};
    inline std::mutex queue_mutex{};
    inline std::priority_queue<request_t> request_queue{};
    inline std::vector<std::vector<int>> groups{};

    inline int group_size = 0;
    inline int guides_capacity = 0;
    inline int guides_count = 0;

    inline
}

#endif
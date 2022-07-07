#pragma once

#include <cstdint>
#include <chrono>

namespace simv3
{
    // nano seconds resolution
    using Clock = std::chrono::nanoseconds;

    struct MessageHeader
    {
        simv3::Clock clock;    // 1st sort key
        int64_t sequence{-1};  // 2nd sort key
        int32_t key{-1};       // e.g., stock id
        int32_t type{-1};      // user defined message type id
        int32_t size{0};       // message size in bytes
        int32_t from_node{-1}; // sender's actor id, -1 for source

        bool operator<(const MessageHeader &rhs) const
        {
            return this->clock < rhs.clock ||
                   (this->clock == rhs.clock && this->sequence < rhs.sequence);
        }

        bool operator>(const MessageHeader &rhs) const
        {
            return this->clock > rhs.clock ||
                   (this->clock == rhs.clock && this->sequence > rhs.sequence);
        }
    };
} // namespace simv3
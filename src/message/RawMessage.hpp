#pragma once

#include "MessageHeader.hpp"
#include "MessageDef.hpp"

#include <cstring>
#include <memory>

namespace simv3
{
    // message shall own its payload
    struct RawMessage
    {
    public:
        RawMessage() = default;

        RawMessage(const MessageHeader &header,
                   const uint8_t *payload)
        {
            header_ = header;
            payload_ = std::shared_ptr<uint8_t[]>(new uint8_t[header_.size]);
            memcpy(this->payload_.get(), payload, header_.size);
        }

        // this will modify the header
        template <typename MsgT>
        RawMessage(MessageHeader &header, const MsgT &msg)
        {
            static_assert(MsgTrait<MsgT>::type != -1, "message type is not registered!");
            static_assert(std::is_trivially_copyable<MsgT>::value);

            header.type = MsgTrait<MsgT>::type;
            header.size = sizeof(MsgT);
            header_ = header;

            payload_ = std::shared_ptr<uint8_t[]>(new uint8_t[header_.size]);
            memcpy(this->payload_.get(), &msg, header_.size);
        }

        const MessageHeader &header() const
        {
            return header_;
        }

        // we can modify header
        MessageHeader &header()
        {
            return header_;
        }

        const uint8_t *payload() const
        {
            return payload_.get();
        }

        // copyable and movable

    private:
        MessageHeader header_;
        std::shared_ptr<uint8_t[]> payload_;
    };
} // namespace simv3

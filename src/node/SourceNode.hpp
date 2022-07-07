#pragma once

#include "Node.hpp"
#include "../message/RawMessage.hpp"

namespace dag
{
    class RawMessageHandler
    {
    public:
        virtual ~RawMessageHandler() = default;
        void on_raw_message(const simv3::RawMessage &raw_msg)
        {
            auto iter = handlers_.find(raw_msg.header().type);
            if (iter != handlers_.end())
            {
                iter->second(raw_msg);
            }
        }

    protected:
        // message id to callbacks
        std::unordered_map<int32_t, std::function<void(const simv3::RawMessage &)>> handlers_;
    };

    template <typename... ChildTypes>
    class SourceNode : public Node<ChildTypes...>, public RawMessageHandler
    {
    public:
        SourceNode(const std::string &name, const std::vector<std::string> &parent_names)
            : Node<ChildTypes...>(name, parent_names)
        {
            if (parent_names.empty() == false)
            {
                throw std::invalid_argument("source node expect 0 parent but get " +
                                            std::to_string(parent_names.size()));
            }
        }

    protected:
        // call this method to register whatever message you want to handle
        template <typename MsgType>
        void registerMessageType()
        {
            static_assert(simv3::MsgTrait<MsgType>::type != -1, "message type is not registered!");

            auto handler = [this](const simv3::RawMessage &raw_msg)
            {
                const auto &header = raw_msg.header();
                const auto &msg = *reinterpret_cast<const MsgType *>(raw_msg.payload());
                this->notify_children(msg, header.clock, header.sequence, header.key);
            };

            handlers_.emplace(simv3::MsgTrait<MsgType>::type, handler);
        }
    };
} // namespace dag

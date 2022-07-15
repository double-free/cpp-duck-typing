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
        SourceNode(int key, const std::string &name, const std::vector<std::string> &parent_names)
            : Node<ChildTypes...>(key, name, parent_names)
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
            constexpr int msg_type = simv3::MsgTrait<MsgType>::type;
            static_assert(msg_type != -1, "message type is not registered!");

            if (handlers_.find(msg_type) != handlers_.end())
            {
                throw std::invalid_argument("register duplicated message type: " +
                                            std::to_string(msg_type));
            }

            auto handler = [this](const simv3::RawMessage &raw_msg)
            {
                const auto &msg = *reinterpret_cast<const MsgType *>(raw_msg.payload());
                this->template notify_children(msg, raw_msg.header().clock);
            };

            handlers_.emplace(msg_type, handler);
        }
    };
} // namespace dag

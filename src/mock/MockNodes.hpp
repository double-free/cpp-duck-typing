#pragma once

#include "../node/Node.hpp"
#include "../node/SourceNode.hpp"

#include <vector>

namespace simv3
{
    struct MockMessage
    {
        int x{0};
    };

    SIM_REGISTER_MSG(MockMessage, 777)
} // namespace simv3

namespace ut
{
    class MockSinkNode : public dag::Node<>
    {
    public:
        MockSinkNode(
            int key,
            const std::string &name,
            const std::vector<std::string> &parent_names)
            : dag::Node<>(key, name, parent_names)
        {
        }

        template <typename MsgT>
        void on_msg_received(const simv3::MessageHeader &header, const MsgT &msg)
        {
            headers_.push_back(header);
        }

        const std::vector<simv3::MessageHeader> &received_headers() const
        {
            return headers_;
        }

    private:
        std::vector<simv3::MessageHeader> headers_;
    };

    class MockFeatureNode : public dag::Node<MockSinkNode>
    {
    public:
        MockFeatureNode(
            int key,
            const std::string &name,
            const std::vector<std::string> &parent_names)
            : dag::Node<MockSinkNode>(key, name, parent_names)
        {
        }
        void on_msg_received(const simv3::MessageHeader &header,
                             const simv3::MockMessage &msg);
    };

    class MockFeatureNode2 : public dag::Node<MockSinkNode>
    {
    public:
        MockFeatureNode2(
            int key,
            const std::string &name,
            const std::vector<std::string> &parent_names)
            : dag::Node<MockSinkNode>(key, name, parent_names)
        {
        }
        void on_msg_received(const simv3::MessageHeader &header,
                             const simv3::MockMessage &msg);
    };

    class MockSourceNode : public dag::SourceNode<MockFeatureNode>
    {
    public:
        MockSourceNode(
            int key,
            const std::string &name,
            const std::vector<std::string> &parent_names)
            : dag::SourceNode<MockFeatureNode>(key, name, parent_names)
        {
            registerMessageType<simv3::MockMessage>();
        }
    };

    class MockSourceNode2 : public dag::SourceNode<MockFeatureNode2>
    {
    public:
        MockSourceNode2(
            int key,
            const std::string &name,
            const std::vector<std::string> &parent_names)
            : dag::SourceNode<MockFeatureNode2>(key, name, parent_names)
        {
            registerMessageType<simv3::MockMessage>();
        }
    };
} // namespace ut

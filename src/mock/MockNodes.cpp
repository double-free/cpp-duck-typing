#include "MockNodes.hpp"

namespace ut
{
    void MockFeatureNode::on_msg_received(const simv3::MessageHeader &header,
                                          const simv3::MockMessage &msg)
    {
        auto msg2 = msg;
        msg2.x += 1;
        notify_children(msg2, header.clock + std::chrono::microseconds(1));
    }

    void MockFeatureNode2::on_msg_received(const simv3::MessageHeader &header,
                                           const simv3::MockMessage &msg)
    {
        notify_children(msg, header.clock + std::chrono::microseconds(10));
    }
} // namespace ut

namespace ut
{
    REGISTER_OBJECT(dag::NodeFactory, MockSourceNode);
    REGISTER_OBJECT(dag::NodeFactory, MockSourceNode2);
    REGISTER_OBJECT(dag::NodeFactory, MockFeatureNode);
    REGISTER_OBJECT(dag::NodeFactory, MockFeatureNode2);
    REGISTER_OBJECT(dag::NodeFactory, MockSinkNode);
} // namespace ut

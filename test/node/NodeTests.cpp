#include "NodeTestFixture.hpp"

TEST_F(NodeTestFixture, parent_id)
{
    // sink node's parent is feature node
    ASSERT_EQ(1, sink_node_->parents().size());
    EXPECT_EQ(feature_node_->node_id(), sink_node_->parents()[0]->node_id());

    // feature node's parent is source node
    ASSERT_EQ(1, feature_node_->parents().size());
    EXPECT_EQ(source_node_->node_id(), feature_node_->parents()[0]->node_id());

    // source node has no parent
    ASSERT_EQ(0, source_node_->parents().size());
}

TEST_F(NodeTestFixture, message_flow)
{
    simv3::MessageHeader header;
    header.clock = simv3::Clock(std::chrono::seconds(10));
    header.sequence = 0;
    header.key = 10000;

    simv3::MockMessage msg;
    auto raw_message = simv3::RawMessage(header, msg);

    source_node_->on_raw_message(raw_message);

    const auto &headers = sink_node_->received_headers();

    // sink node received the message
    ASSERT_EQ(1, headers.size());
    const auto &received_header = headers[0];

    // the message is modified by feature node
    EXPECT_EQ(std::chrono::seconds(10) + std::chrono::microseconds(1), received_header.clock);
    EXPECT_EQ(1, received_header.sequence);
    EXPECT_EQ(10000, received_header.key);
    EXPECT_EQ(simv3::MsgTrait<simv3::MockMessage>::type, received_header.type);
    EXPECT_EQ(sizeof(simv3::MockMessage), received_header.size);
}
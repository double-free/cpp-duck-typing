#pragma once

#include "../../src/mock/MockNodes.hpp"

#include <gtest/gtest.h>

class NodeTestFixture : public testing::Test
{
public:
    void SetUp() final
    {

        sink_node_ = std::make_shared<ut::MockSinkNode>(
            "sink",
            std::vector<std::string>({"feature"}));
        EXPECT_EQ(0, sink_node_->expected_children_count());

        feature_node_ = std::make_shared<ut::MockFeatureNode>(
            "feature",
            std::vector<std::string>({"source"}));
        EXPECT_EQ(0, feature_node_->add_child(sink_node_));
        EXPECT_EQ(1, feature_node_->expected_children_count());

        source_node_ = std::make_shared<ut::MockSourceNode>(
            "source",
            std::vector<std::string>({}));
        EXPECT_EQ(0, source_node_->add_child(feature_node_));
        EXPECT_EQ(1, source_node_->expected_children_count());
    }

protected:
    std::shared_ptr<ut::MockSourceNode> source_node_;
    std::shared_ptr<ut::MockSinkNode> sink_node_;
    std::shared_ptr<ut::MockFeatureNode> feature_node_;
};
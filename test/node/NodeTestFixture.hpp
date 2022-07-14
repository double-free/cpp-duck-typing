#pragma once

#include "../../src/mock/MockNodes.hpp"

#include <gtest/gtest.h>

class NodeTestFixture : public testing::Test
{
public:
    void SetUp() final
    {
        int key = 10000;
        sink_node_ = std::make_shared<ut::MockSinkNode>(
            key,
            "sink",
            std::vector<std::string>({"feature"}));
        EXPECT_EQ(0, sink_node_->children().size());

        feature_node_ = std::make_shared<ut::MockFeatureNode>(
            key,
            "feature",
            std::vector<std::string>({"source"}));
        EXPECT_EQ(0, feature_node_->add_child(sink_node_));
        EXPECT_EQ(1, feature_node_->children().size());

        source_node_ = std::make_shared<ut::MockSourceNode>(
            key,
            "source",
            std::vector<std::string>({}));
        EXPECT_EQ(0, source_node_->add_child(feature_node_));
        EXPECT_EQ(1, source_node_->children().size());
    }

protected:
    std::shared_ptr<ut::MockSourceNode> source_node_;
    std::shared_ptr<ut::MockSinkNode> sink_node_;
    std::shared_ptr<ut::MockFeatureNode> feature_node_;
};
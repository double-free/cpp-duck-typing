#include "node/Node.hpp"
#include "node/SourceNode.hpp"
#include "mock/MockNodes.hpp"

int main(int argc, char const *argv[])
{
  int key = 10000;
  std::shared_ptr<dag::NodeBase> sink = dag::NodeFactory::create("MockSinkNode", key, "sink", {"feature"});

  // node #1
  std::shared_ptr<dag::NodeBase> node = dag::NodeFactory::create("MockFeatureNode", key, "feature", {"source"});
  node->add_child(sink);

  // node #2
  std::shared_ptr<dag::NodeBase> source = dag::NodeFactory::create("MockSourceNode", key, "source", {});
  source->add_child(node);

  simv3::MessageHeader header;
  header.clock = simv3::Clock(std::chrono::seconds(10));
  header.sequence = 0;
  header.key = 1111;

  simv3::MockMessage msg;
  auto raw_message = simv3::RawMessage(header, msg);

  auto casted = std::dynamic_pointer_cast<dag::RawMessageHandler>(source);
  casted->on_raw_message(raw_message);

  return 0;
}

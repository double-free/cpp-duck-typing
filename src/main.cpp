#include "concrete_node.hpp"

namespace dag {
std::atomic<int> node_index = 0;
} // namespace dag

int main(int argc, char const *argv[]) {
  auto sink = std::make_shared<dag::SinkNode>();

  // node #1
  auto node = std::make_shared<dag::NodeA>();
  node->set_child<0>(sink);

  // node #2
  auto source = std::make_shared<dag::SourceNode>();
  source->set_child<0>(node);

  source->notify_children(10);

  return 0;
}

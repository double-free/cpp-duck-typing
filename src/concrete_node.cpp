#include "concrete_node.hpp"

#include <iostream>
#include <typeinfo>

using namespace dag;

void SinkNode::onMsgReceived(const double &msg) {
  std::cout << "msg with type [" << typeid(msg).name()
            << "] reaches SinkNode\n";
}

template <> void NodeA ::onMsgReceived(const int &msg) {
  std::cout << "msg with type [" << typeid(msg).name()
            << "] reaches NodeA: " << node_id() << "\n";

  double y = 3.0;
  notifyChildren(y);
}

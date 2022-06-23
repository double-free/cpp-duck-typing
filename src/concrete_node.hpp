#pragma once

#include "node.hpp"

namespace dag {
// source node has no parent
using SourceNode = WithParents<>::Node<NodeA>;
// sink node has no child
using SinkNode = WithParents<NodeA>::Node<>;

using NodeA = WithParents<SourceNode>::Node<SinkNode>;

} // namespace dag

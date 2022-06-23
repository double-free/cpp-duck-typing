#pragma once

#include "node.hpp"

namespace dag {
// source node has no parent
using SourceNode = WithParent<>::WithChild<NodeA>::Node;
// sink node has no child
using SinkNode = WithParent<NodeA>::WithChild<>::Node;

using NodeA = WithParent<SourceNode>::WithChild<SinkNode>::Node;

} // namespace dag

#include "node.hpp"

int main(int argc, char const *argv[])
{
    dag::SinkNode sink;

    dag::NodeA node(sink);

    dag::SourceNode source(node);

    source.notifyChildren(10);

    return 0;
}

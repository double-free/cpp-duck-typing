#include "node.hpp"

int main(int argc, char const *argv[])
{
    dag::SinkNode sink;

    dag::NodeA node(&sink);

    return 0;
}

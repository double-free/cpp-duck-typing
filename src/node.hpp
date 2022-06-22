#pragma once

#include <tuple>

#include <iostream>

namespace dag
{
    template <typename... ChildTypes>
    class Node
    {
    public:
        explicit Node(ChildTypes... children)
            : children_(std::forward_as_tuple(children...))
        {
            static int node_id = 0;
            node_id_ = ++node_id;
        }

        // do nothing by default
        template <typename MsgT>
        void onMsgReceived(const MsgT &msg)
        {
        }

        template <typename MsgT>
        void notifyChildren(const MsgT &msg)
        {
            std::apply([&msg](auto &&...children)
                       { ((children.onMsgReceived(msg)), ...); },
                       children_);
        }

        int node_id() const
        {
            return node_id_;
        }

    protected:
        std::tuple<ChildTypes...> children_;

    private:
        int node_id_{0};
    };

    // node with no child
    class SinkNode : public Node<>
    {
    public:
        template <typename MsgT>
        void onMsgReceived(const MsgT &msg)
        {
            std::cout << "msg reaches SinkNode: " << node_id() << "\n";
        }
    };

    // node with one child "SinkNode"
    class NodeA : public Node<SinkNode>
    {
    public:
        explicit NodeA(SinkNode node)
            : Node(node)
        {
        }

        template <typename MsgT>
        void onMsgReceived(const MsgT &msg)
        {
            std::cout << "msg reaches NodeA: " << node_id() << "\n";

            double y = 3.0;
            notifyChildren(y);
        }
    };

    class SourceNode : public Node<NodeA>
    {
    public:
        explicit SourceNode(NodeA node)
            : Node(node)
        {
        }
    };

} // namespace dag

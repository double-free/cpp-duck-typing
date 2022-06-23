#pragma once

#include <tuple>
#include <memory>
#include <typeinfo>
#include <atomic>

#include <iostream>

namespace dag
{
    extern std::atomic<int> node_index;

    template <typename... ChildTypes>
    class Node
    {
    public:
        Node()
        {
            node_id_ = ++node_index;
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
                       { ((children->onMsgReceived(msg)), ...); },
                       children_);
        }

        template <size_t index, typename ChildT>
        void set_child(std::shared_ptr<ChildT> child)
        {
            std::get<index>(children_) = child;
        }

        int node_id() const
        {
            return node_id_;
        }

    protected:
        std::tuple<std::shared_ptr<ChildTypes>...> children_;

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
            std::cout << "msg with type [" << typeid(msg).name() << "] reaches SinkNode: " << node_id() << "\n";
        }
    };

    // node with one child "SinkNode"
    class NodeA : public Node<SinkNode>
    {
    public:
        template <typename MsgT>
        void onMsgReceived(const MsgT &msg)
        {
            std::cout << "msg with type [" << typeid(msg).name() << "] reaches NodeA: " << node_id() << "\n";

            double y = 3.0;
            notifyChildren(y);
        }
    };

    class SourceNode : public Node<NodeA>
    {
    };

} // namespace dag

#pragma once

#include "../message/RawMessage.hpp"
#include "../util/Factory.hpp"

#include <tuple>
#include <memory>
#include <typeinfo>
#include <vector>

namespace dag
{
    // primitive node type, don't use it directly
    class NodeBase
    {
    public:
        NodeBase(const std::string &name,
                 const std::vector<std::string> &parent_names);

        virtual ~NodeBase() = default;

        virtual int add_child(std::shared_ptr<NodeBase> child)
        {
            return -1;
        }

        virtual size_t expected_children_count() const
        {
            return 0;
        }

        const std::string &name() const
        {
            return name_;
        }

        int node_id() const
        {
            return node_id_;
        }

        void add_parent(const NodeBase &parent);

        const auto &parents() const
        {
            return parents_;
        }

    private:
        std::string name_;
        std::vector<std::string> parent_names_;
        std::vector<const NodeBase *> parents_;
        int node_id_{0};
    };

    // template parameters are the children
    // TODO: how to access the parents?
    template <typename... ChildTypes>
    class Node : public NodeBase
    {
        using ChildTypesTuple = std::tuple<ChildTypes...>;

    protected:
        std::tuple<std::shared_ptr<ChildTypes>...> children_;

    public:
        Node(const std::string &name,
             const std::vector<std::string> &parent_names)
            : NodeBase(name, parent_names)
        {
        }

        // do nothing by default
        template <typename MsgT>
        void on_msg_received(const simv3::MessageHeader &header, const MsgT &msg)
        {
        }

        size_t expected_children_count() const final
        {
            return sizeof...(ChildTypes);
        }

        template <size_t index>
        auto &get_child()
        {
            return std::get<index>(children_);
        }

        template <size_t index>
        const auto &get_child() const
        {
            return std::get<index>(children_);
        }

        // return the index of the child been set, -1 if invalid
        int add_child(std::shared_ptr<NodeBase> child) final
        {
            if (child == nullptr)
            {
                return -1;
            }
            return add_child_helper<0>(child);
        }

    protected:
        template <typename MsgT>
        void notify_children(const MsgT &msg, simv3::Clock clock, int64_t sequence, int32_t key)
        {
            static_assert(std::is_trivially_copyable<MsgT>::value);

            simv3::MessageHeader header;
            header.clock = clock;
            header.sequence = sequence;
            header.key = key;

            header.from_node = node_id();
            header.type = simv3::MsgTrait<MsgT>::type;
            header.size = sizeof(MsgT);

            std::apply([&header, &msg](auto &&...child)
                       { ((child->on_msg_received(header, msg)), ...); },
                       children_);
        }

    private:
        template <size_t index>
        typename std::enable_if_t<index == sizeof...(ChildTypes), int>
        add_child_helper(std::shared_ptr<NodeBase> &child);
        template <size_t index>
        typename std::enable_if_t<(index < sizeof...(ChildTypes)), int>
        add_child_helper(std::shared_ptr<NodeBase> &child);
    };

    template <typename... ChildTypes>
    template <size_t index>
    typename std::enable_if_t<index == sizeof...(ChildTypes), int>
    Node<ChildTypes...>::add_child_helper(std::shared_ptr<NodeBase> &child)
    {
        return -1;
    }

    template <typename... ChildTypes>
    template <size_t index>
    typename std::enable_if_t<(index < sizeof...(ChildTypes)), int>
    Node<ChildTypes...>::add_child_helper(std::shared_ptr<NodeBase> &child)
    {
        using ExpectType = std::tuple_element_t<index, ChildTypesTuple>;

        // must be the same type, don't use dynamic cast
        // because dynamic cast is a loose requirement
        // e.g., A <- B <- C
        // A* c =  new C()
        // you can also successfully dynamic_cast c to B*

        auto &current_child = std::get<index>(children_);
        if (current_child != nullptr)
        {
            // don't set the same child twice
            return add_child_helper<index + 1>(child);
        }

        if (typeid(ExpectType) != typeid(*child))
        {
            // type is different to given one
            return add_child_helper<index + 1>(child);
        }

        current_child = std::dynamic_pointer_cast<ExpectType>(child);
        current_child->add_parent(*this);
        return index;
    }

    using NodeFactory = yy::StaticFactory<NodeBase,
                                          const std::string &,
                                          const std::vector<std::string> &>;
} // namespace dag

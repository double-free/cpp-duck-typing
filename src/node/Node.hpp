#pragma once

#include "../message/RawMessage.hpp"
#include "../util/Factory.hpp"

#include <tuple>
#include <memory>
#include <typeinfo>
#include <vector>
#include <set>

namespace dag
{
    // primitive node type, don't use it directly
    class NodeBase
    {
    public:
        // key is usually security id
        NodeBase(int key,
                 const std::string &name,
                 const std::vector<std::string> &parent_names);

        virtual ~NodeBase() = default;

        virtual int add_child(std::shared_ptr<NodeBase> child)
        {
            return -1;
        }
        virtual std::vector<std::shared_ptr<NodeBase>> children() const
        {
            return {};
        }

        /////////////////
        // snap to csv //
        /////////////////
        // keys will be used to construct csv header in snap
        // please make sure keys match values!!!
        virtual std::vector<std::string> snap_keys() const
        {
            return {};
        }
        // output ',' separated values, example:
        // ss << value1 << "," << value2 << "," << value3 << ",";
        // you must add a trailing ',' at the end
        // if the node is not ready to output values, return false
        virtual bool write_snap_values(std::stringstream &ss) const
        {
            return true;
        }

    public:
        int key() const
        {
            return key_;
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

        template <typename MsgT>
        simv3::MessageHeader generate_header(const simv3::Clock &clock)
        {
            simv3::MessageHeader header;
            header.clock = clock;

            header.sequence = ++msg_sequence_;
            header.key = key_;
            header.from_node = node_id();
            header.type = simv3::MsgTrait<MsgT>::type;
            header.size = sizeof(MsgT);

            return header;
        }

    protected:
        // sequence of msg that is sent by this node
        int64_t msg_sequence_{0};

    private:
        int key_{-1};
        std::string name_;
        std::vector<std::string> parent_names_;
        std::vector<const NodeBase *> parents_;
        int node_id_{0};
    };

    // template parameters are the children types
    // children types shall be unique
    // TODO: how to access the parents?
    template <typename... ChildTypes>
    class Node : public NodeBase
    {
        using ChildTypesTuple = std::tuple<ChildTypes...>;

    protected:
        std::tuple<std::vector<std::shared_ptr<ChildTypes>>...> children_;

    public:
        Node(int key,
             const std::string &name,
             const std::vector<std::string> &parent_names)
            : NodeBase(key, name, parent_names)
        {
            std::set<size_t> children_types{typeid(ChildTypes).hash_code()...};
            if (children_types.size() < sizeof...(ChildTypes))
            {
                throw std::invalid_argument(
                    name + " has duplicated children type!");
            }
        }

        std::vector<std::shared_ptr<NodeBase>> children() const final
        {
            std::vector<std::shared_ptr<NodeBase>> result;
            std::apply([&result](auto &&...child_vec)
                       { ((result.insert(result.end(),
                                         child_vec.begin(), child_vec.end())),
                          ...); },
                       children_);
            return result;
        }

        // do nothing by default
        template <typename MsgT>
        void on_msg_received(const simv3::MessageHeader &header, const MsgT &msg)
        {
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
        // notify children with adhoc message, skip storing message
        template <typename MsgT>
        void notify_children(const MsgT &msg, simv3::Clock clock)
        {
            auto header = generate_header<MsgT>(clock);
            notify_children_helper(header, msg, std::index_sequence_for<ChildTypes...>{});
        }

    private:
        template <size_t index>
        typename std::enable_if_t<index == sizeof...(ChildTypes), int>
        add_child_helper(std::shared_ptr<NodeBase> &child)
        {
            return -1;
        }
        template <size_t index>
        typename std::enable_if_t<(index < sizeof...(ChildTypes)), int>
        add_child_helper(std::shared_ptr<NodeBase> &child);

        template <typename MsgT, std::size_t... Is>
        void notify_children_helper(const simv3::MessageHeader &header,
                                    const MsgT &msg,
                                    std::index_sequence<Is...>);
    };

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

        if (typeid(ExpectType) != typeid(*child))
        {
            // type is different to given one
            return add_child_helper<index + 1>(child);
        }

        child->add_parent(*this);
        auto &child_vec = std::get<index>(children_);
        child_vec.push_back(std::dynamic_pointer_cast<ExpectType>(child));
        return index;
    }

    template <typename NodeT, typename MsgT>
    void notify_each(std::vector<std::shared_ptr<NodeT>> &child_vec,
                     const simv3::MessageHeader &header,
                     const MsgT &msg)
    {
        std::for_each(child_vec.begin(), child_vec.end(),
                      [&header, &msg](std::shared_ptr<NodeT> &node)
                      {
                          node->on_msg_received(header, msg);
                      });
    }

    template <typename... ChildTypes>
    template <typename MsgT, std::size_t... Is>
    void Node<ChildTypes...>::notify_children_helper(const simv3::MessageHeader &header,
                                                     const MsgT &msg,
                                                     std::index_sequence<Is...>)
    {
        ((notify_each(std::get<Is>(children_), header, msg)), ...);
    }

    using NodeFactory = yy::StaticFactory<NodeBase,
                                          const int &,
                                          const std::string &,
                                          const std::vector<std::string> &>;
} // namespace dag

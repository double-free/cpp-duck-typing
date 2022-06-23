#pragma once

#include <tuple>
#include <memory>
#include <typeinfo>
#include <atomic>

#include <iostream>

namespace dag
{
    extern std::atomic<int> node_index;

    // template parameters are the children
    // TODO: how to access the parents?
    template <typename... ParentTypes>
    class WithParents
    {
        template <typename... ChildTypes>
        struct Node
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
            void notifyChildren(const MsgT &msg) const
            {
                std::apply([&msg](auto &&...children)
                           { ((children->onMsgReceived(msg)), ...); },
                           children_);
            }

            template <size_t index, typename ChildT>
            void set_child(std::shared_ptr<ChildT> child)
            {
                std::get<index>(children_) = child;
                // template as a qualifier
                child->template set_parent<index>(this);
            }

            // not expected to be called directly
            template <size_t index, typename ParentT>
            void set_parent(const ParentT *parent)
            {
                std::get<index>(parents_) = parent;
            }

            int node_id() const
            {
                return node_id_;
            }

        protected:
            std::tuple<ParentTypes *...> parents_;
            std::tuple<std::shared_ptr<ChildTypes>...> children_;

        private:
            int node_id_{0};
        };
    };

} // namespace dag

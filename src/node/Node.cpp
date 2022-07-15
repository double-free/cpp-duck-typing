#include "Node.hpp"

#include <atomic>

namespace dag
{
    std::atomic<int> GLOBAL_NODE_INDEX = 0;

    NodeBase::NodeBase(
        int key,
        const std::string &name,
        const std::vector<std::string> &parent_names)
        : key_(key), name_(name), parent_names_(parent_names)
    {
        parents_.resize(parent_names.size());
        node_id_ = ++GLOBAL_NODE_INDEX;
    }

    void NodeBase::add_parent(const NodeBase &parent)
    {
        for (size_t i = 0; i < parent_names_.size(); ++i)
        {
            if (parent.name() == parent_names_[i])
            {
                if (parents_[i] != nullptr)
                {
                    throw std::invalid_argument(
                        this->name() + " set duplicated parents. prev: " +
                        parents_[i]->name() + ", now: " + parent.name());
                }
                parents_[i] = &parent;
                return;
            }
        }

        throw std::invalid_argument(this->name() +
                                    " does not expect parent with name: " +
                                    parent.name());
    }
} // namespace dag

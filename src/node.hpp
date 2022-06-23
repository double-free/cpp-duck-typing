#pragma once

#include <atomic>
#include <memory>
#include <tuple>

namespace dag {
extern std::atomic<int> node_index;

// template parameters are the children
// TODO: how to access the parents?
template <typename... ParentTypes> struct WithParents {
  template <typename... ChildTypes> struct Node {
  public:
    Node() { node_id_ = ++node_index; }

    // do nothing by default
    template <typename MsgT> void onMsgReceived(const MsgT &msg) {}

    template <typename MsgT> void notifyChildren(const MsgT &msg) const;

    template <size_t index, typename ChildT>
    void set_child(std::shared_ptr<ChildT> child);

    // not expected to be called directly
    template <size_t index, typename ParentT>
    void set_parent(const ParentT *parent);

    int node_id() const { return node_id_; }

  protected:
    std::tuple<ParentTypes *...> parents_;
    std::tuple<std::shared_ptr<ChildTypes>...> children_;

  private:
    int node_id_{0};
  };
};

template <typename... ParentTypes>
template <typename... ChildTypes>
template <size_t index, typename ChildT>
void WithParents<ParentTypes...>::Node<ChildTypes...>::set_child(
    std::shared_ptr<ChildT> child) {
  std::get<index>(children_) = child;
  // template as a qualifier
  child->template set_parent<index>(this);
}

template <typename... ParentTypes>
template <typename... ChildTypes>
template <size_t index, typename ParentT>
void WithParents<ParentTypes...>::Node<ChildTypes...>::set_parent(
    const ParentT *parent) {
  std::get<index>(parents_) = parent;
}

template <typename... ParentTypes>
template <typename... ChildTypes>
template <typename MsgT>
void WithParents<ParentTypes...>::Node<ChildTypes...>::notifyChildren(
    const MsgT &msg) const {
  std::apply(
      [&msg](auto &&...children) { ((children->onMsgReceived(msg)), ...); },
      children_);
}
} // namespace dag

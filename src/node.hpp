#pragma once

#include <atomic>
#include <memory>
#include <tuple>

namespace dag {
extern std::atomic<int> node_index;

template <typename ParentTuple, typename ChildTuple> class _Node {
public:
  _Node() { node_id_ = ++node_index; }

  // do nothing by default
  template <typename MsgT> void on_msg_received(const MsgT &msg) {}

  template <typename MsgT> void notify_children(const MsgT &msg) const {
    std::apply(
        [&msg](auto &&...children) { ((children->on_msg_received(msg)), ...); },
        children_);
  }

  template <size_t index, typename ChildT>
  void set_child(std::shared_ptr<ChildT> child);

  // not expected to be called directly
  template <size_t index, typename ParentT>
  void set_parent(const ParentT *parent);

  int node_id() const { return node_id_; }

protected:
  ParentTuple parents_;
  ChildTuple children_;

private:
  int node_id_{0};
};

template <typename ParentTuple, typename ChildTuple>
template <size_t index, typename ChildT>
void _Node<ParentTuple, ChildTuple>::set_child(std::shared_ptr<ChildT> child) {
  std::get<index>(children_) = child;
  // template as a qualifier
  child->template set_parent<index>(this);
}

template <typename ParentTuple, typename ChildTuple>
template <size_t index, typename ParentT>
void _Node<ParentTuple, ChildTuple>::set_parent(const ParentT *parent) {
  std::get<index>(parents_) = parent;
}

// template parameters are the children
// TODO: how to access the parents?
template <typename... ParentTypes> struct WithParent {
  template <typename... ChildTypes> struct WithChild {
    using Node = _Node<std::tuple<ParentTypes *...>,
                       std::tuple<std::shared_ptr<ChildTypes>...>>;
  };
};

} // namespace dag

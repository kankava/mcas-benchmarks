//
// Lock-free Doubly-Linked List implementation using the algorithm
// described A Pragmatic Implementation of Non-blocking Linked-Lists paper
//

#pragma once

#include <memory>
#include <mutex>
#include "../mcas/mcas.h"

namespace lockfree_mcas {

template <typename T>
class SortedList {
 private:
  struct Node {
    // int marked;
    std::shared_ptr<T> data;
    std::shared_ptr<Node> next;
    std::shared_ptr<Node> prev;
    Node() = default;
  };

  std::shared_ptr<Node> head;
  std::shared_ptr<Node> tail;
  std::mutex cas_lock = {};

 public:
  SortedList() {
    head = std::make_shared<Node>();
    tail = std::make_shared<Node>();
    head->next = tail;
    tail->prev = head;
  }

  void insert(T const& data) {
    std::shared_ptr<Node> const new_node = std::make_shared<Node>();
    new_node->next = nullptr;
    new_node->prev = nullptr;
    new_node->data = std::make_shared<T>(data);

    while (true) {
      auto parent = head;
      auto child = head->next;

      // search position (buggy???)
      while (child != nullptr && *child->data < data) {
        parent = child;
        child = child->next;
      }

      if (child == nullptr) {
        // parent->next = new_node;
        // new_node->prev = parent;
        auto pnext = parent->next;
        auto nprev = new_node->prev;
        {
          std::lock_guard<std::mutex> lock(cas_lock);
          if (DCAS(&parent->next, &new_node->prev,
                   pnext, nprev,
                   new_node, parent))
            return;
        }
      }

      // parent->next = new_node;
      // child->prev = new_node;
      // new_node->prev = parent;
      // new_node->next = child;

      auto pnext = parent->next;
      auto cprev = child->prev;
      auto nprev = new_node->prev;
      auto nnext = new_node->next;
      {
        std::lock_guard<std::mutex> lock(cas_lock);
        if (CAS4(&parent->next, &child->prev, &new_node->prev, &new_node->next,
              pnext, cprev, nprev, nnext,
              new_node, new_node, parent, child))
          return;
      }
    }
  }

  void remove(T v) {
    while (true) {
      std::shared_ptr<Node> parent = head;
      std::shared_ptr<Node> current = head->next;

      // search node (buggy???)
      while (current != nullptr && *current->data < v) {
        parent = current;
        current = current->next;
      }

      if (current == nullptr || *current->data != v) return;

      auto child = current->next;

      if (child == nullptr) {
        auto pnext = parent->next;
        {
          std::lock_guard<std::mutex> lock(cas_lock);
          if (CAS(&parent->next, pnext, child)) return;
        }
      } else {
        auto pnext = parent->next;
        auto gcprev = child->prev;

        if (DCAS(&parent->next, &child->prev, pnext, gcprev, child, parent))
          return;
      }
    }
  }

  int count(T val) {
    auto current = head->next;
    int c = 0;

    // iterate over nodes (buggy???)
    while (current != nullptr) {
      if (*current->data == val) c++;
      current = current->next;
    }

    return c;
  }
};

}  // namespace lockfree_mcas

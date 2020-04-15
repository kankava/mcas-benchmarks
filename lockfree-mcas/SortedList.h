#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include "../mcas/mcas.h"

namespace lockfree_mcas {

template <typename T>
class SortedList {
 private:
  struct Node {
    std::shared_ptr<T> data;
    std::shared_ptr<Node> next;
    std::shared_ptr<Node> prev;
    Node() = default;
  };

  std::shared_ptr<Node> head;
  std::shared_ptr<Node> tail;
  std::shared_ptr<Node> dummy;
  std::mutex cas_lock = {};

 public:
  SortedList() {
    head = std::make_shared<Node>();
    tail = std::make_shared<Node>();
    dummy = std::make_shared<Node>();
    head->next = tail;
    tail->prev = head;
  }

  void insert(T const& data) {
    std::shared_ptr<Node> const new_node = std::make_shared<Node>();
    new_node->data = std::make_shared<T>(data);
    new_node->next = dummy;
    new_node->prev = dummy;

    while (true) {
      auto parent = head;
      auto curr = head->next;

      while (curr != nullptr && curr->data != nullptr && *curr->data < data) {
        parent = curr;
        curr = curr->next;
      }

      {
        std::lock_guard<std::mutex> lock(cas_lock);
        if (CAS4(&parent->next, &curr->prev, &new_node->next, &new_node->prev,
                  curr, parent, dummy, dummy,
                  new_node, new_node, curr, parent))
          return;
      }
    }
  }

  void remove(T const& data) {
    while (true)
      while (true) {
        std::shared_ptr<Node> curr = head->next;

        while (curr != tail && curr->data != nullptr && *curr->data < data) {
          curr = curr->next;
        }

        if (curr == tail) return;
        if (curr->data == nullptr) break;
        if (*curr->data != data) return;

        auto child = curr->next;

        std::shared_ptr<Node> c_n_prev = curr->next->prev;
        std::shared_ptr<Node> c_p_next = curr->prev->next;

        {
          std::lock_guard<std::mutex> lock(cas_lock);
          if (DCAS(&curr->next->prev, &curr->prev->next,
                    c_n_prev, c_p_next,
                    curr->prev, curr->next))
            return;
        }
      }
  }

  int count(T val) {
    int n_val = 0;

    while (true) {
      auto curr = head->next;
      n_val = 0;

      while (curr != tail && curr->data != nullptr) {
        if (*curr->data == val) n_val++;
        curr = curr->next;
      }
      if (curr == tail) break;
    }
    return n_val;
  }

  // for testing, not safe
  int print_all() {
    auto curr = head->next;

    while (curr != tail) {
      std::cout << *curr->data << " ";
      curr = curr->next;
    }
    std::cout << std::endl;
  }
};

}  // namespace lockfree_mcas

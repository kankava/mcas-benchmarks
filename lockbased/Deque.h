#pragma once

#include <memory>
#include <mutex>
#include "../mcas/mcas.h"

namespace lockbased {

template <typename T>
class Deque {
 private:
  struct Node {
    std::shared_ptr<T> data;
    std::shared_ptr<Node> L;
    std::shared_ptr<Node> R;
    Node() = default;
  };

  std::shared_ptr<Node> head;
  std::shared_ptr<Node> tail;
  std::mutex op_lock = {};

 public:
  Deque() {
    head = std::make_shared<Node>();
    tail = std::make_shared<Node>();

    head->next = tail;
    tail->prev = head;
  }

  // push_left
  void push_front(T const& data) {
    std::lock_guard<std::mutex> lock(op_lock);
    
    std::shared_ptr<Node> const new_node = std::make_shared<Node>();
    new_node->data = std::make_shared<T>(data);
    
    new_node->next = head->next;
    new_node->prev = head;

    head->next->prev = new_node;
    head->next = new_node;
  }

  // push_right
  void push_back(T const& data) {
    std::lock_guard<std::mutex> lock(op_lock);
    
    std::shared_ptr<Node> const new_node = std::make_shared<Node>();
    new_node->data = std::make_shared<T>(data);

    new_node->next = tail;
    new_node->prev = tail->prev;

    tail->prev->next = new_node;
    tail->prev = new_node;
  }

  // pop_left
  std::shared_ptr<T> pop_front() {
    std::lock_guard<std::mutex> lock(op_lock);
    
    if (head->next != tail) {
      std::shared_ptr<T> data = head->next->data;
      head->next = head->next->next;
      return data;
    }
    return nullptr;
  }

  // pop_right
  std::shared_ptr<T> pop_back() {
    std::lock_guard<std::mutex> lock(op_lock);
    
    if (tail->prev != head) {
      std::shared_ptr<T> data = tail->prev->data;
      tail->prev = tail->prev->prev;
      return data;
    }
    return nullptr;
  }
};

}  // namespace lockbased

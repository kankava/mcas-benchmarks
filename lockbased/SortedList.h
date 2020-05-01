#pragma once

#include <memory>
#include <mutex>

namespace lockbased {

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
  std::mutex list_lock = {};

 public:
  SortedList() {
    head = std::make_shared<Node>();
    tail = std::make_shared<Node>();
    head->next = tail;
    tail->prev = head;
  }

  void insert(T const& data) {
    std::lock_guard<std::mutex> lock(list_lock);

    auto parent = head;
    auto curr = head->next;

    while (curr != tail && *curr->data < data) {
      parent = curr;
      curr = curr->next;
    }

    std::shared_ptr<Node> const new_node = std::make_shared<Node>();
    new_node->data = std::make_shared<T>(data);
    new_node->next = curr;
    new_node->prev = parent;
    
    parent->next = new_node;
    curr->prev = new_node;
    
    return;
  }

  void remove(T const& data) {
    std::lock_guard<std::mutex> lock(list_lock);

    std::shared_ptr<Node> curr = head->next;
    while (curr != tail && *curr->data < data) {
      curr = curr->next;
    }

    if (curr == tail) return;
    if (*curr->data != data) return;
   
    curr->next->prev = curr->prev;
    curr->prev->next = curr->next;

    return;
  }

  int count(T val) {
    std::lock_guard<std::mutex> lock(list_lock);
    
    int n_val = 0;
    auto curr = head->next;
    n_val = 0;

    while (curr != tail) {
      if (*curr->data == val) n_val++;
      curr = curr->next;
    }

    return n_val;
  }

  // for testing, not safe
  void print_all() {
    auto curr = head->next;

    while (curr != tail) {
      std::cout << *curr->data << " ";
      curr = curr->next;
    }
    std::cout << std::endl;
  }
};

}  // namespace lockbased

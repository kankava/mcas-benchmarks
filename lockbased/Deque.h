#pragma once

#include <memory>
#include <mutex>

namespace lockbased {


class Deque {
 private:
  struct Node {
    int data;
    Node *prev;
    Node *next;
    Node() = default;
  };

  Node *head;
  Node *tail;
  std::mutex deque_lock = {};

 public:
  Deque() {
    head = new Node();
    tail = new Node();

    head->next = tail;
    tail->prev = head;
  }

  virtual ~Deque() {
    Node *curr = head;
    while (curr != tail) {
      Node *tmp = curr;
      curr = curr->next;
      delete tmp;
    }
    delete tail;
  }

  // push_left
  void push_front(int const& data) {
    std::lock_guard<std::mutex> lock(deque_lock);
    
    Node *new_node = new Node();
    new_node->data = data;
    
    new_node->next = head->next;
    new_node->prev = head;

    head->next->prev = new_node;
    head->next = new_node;
  }

  // push_right
  void push_back(int const& data) {
    std::lock_guard<std::mutex> lock(deque_lock);
    
    Node *new_node = new Node();
    new_node->data = data;

    new_node->next = tail;
    new_node->prev = tail->prev;

    tail->prev->next = new_node;
    tail->prev = new_node;
  }

  // pop_left
  int pop_front() {
    std::lock_guard<std::mutex> lock(deque_lock);
    
    if (head->next != tail) {
      int data = head->next->data;
      Node *tmp = head->next;
      head->next = head->next->next;
      delete tmp;
      return data;
    }
    return -1;
  }

  // pop_right
  int pop_back() {
    std::lock_guard<std::mutex> lock(deque_lock);
    
    if (tail->prev != head) {
      int data = tail->prev->data;
      Node *tmp = tail->prev;
      tail->prev = tail->prev->prev;
      delete tmp;
      return data;
    }
    return -1;
  }
};

}  // namespace lockbased

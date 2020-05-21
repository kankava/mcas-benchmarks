// Greenwald DCAS Based

#pragma once

#include <climits>
#include <iostream>
#include <memory>
#include <mutex>
#include "../mcas/mcas.h"

namespace lockfree_mcas {

class SortedList {
 private:
  struct Node {
    int data;
    Node *next;
    Node *prev;
    Node() = default;
  };

  Node *head;
  Node *tail;

  bool insert_before(Node *next, Node *node) {
    if (next == head) {
      return insert_after(next, node);
    }

    Node *prev = next->prev;

    node->next = next;
    node->prev = prev;
    if (qcas((uint64_t *)&node->prev->next, (uint64_t)node->next, (uint64_t)node,
             (uint64_t *)&next->prev, (uint64_t)node->prev, (uint64_t)node,
             (uint64_t *)&node->next, (uint64_t)node->next, (uint64_t)next,
             (uint64_t *)&node->prev, (uint64_t)node->prev, (uint64_t)prev
             )
        ) {
      return true;
    } else {
      return false;
    }
  }

  bool insert_after(Node *prev, Node *node) {
    if (prev == tail) {
      return insert_before(prev, node);
    }

    Node *next = prev->next;

    node->prev = prev;
    node->next = next;
    if (qcas((uint64_t *)&prev->next, (uint64_t)node->next, (uint64_t)node,
             (uint64_t *)&node->next->prev, (uint64_t)node->prev, (uint64_t)node,
             (uint64_t *)&node->next, (uint64_t)node->next, (uint64_t)next,
             (uint64_t *)&node->prev, (uint64_t)node->prev, (uint64_t)prev
             )
        ) {
      return true;
    } else {
      return false;
    }
  }

  bool delete_node(Node *node) {
      if (node == head || node == tail) {
        return true;
      }
      Node *prev = node->prev;
      Node *next = node->next;

      if(dcas((uint64_t*)&prev->next, (uint64_t)node, (uint64_t)next,
              (uint64_t*)&next->prev, (uint64_t)node, (uint64_t)prev)) {
        return true;
      } else {
        return false;
      }
  }

 public:
  SortedList() {
    head = new Node();
    tail = new Node();
    head->next = tail;
    tail->prev = head;
  }

  void insert(int data) {
    Node *new_node = new Node();
    new_node->data = data;
    new_node->next = nullptr;
    new_node->prev = nullptr;

    while (true) {
      auto curr = head->next;

      while (curr != tail && curr->data < data) {
        curr = curr->next;
      }
      if (insert_before(curr, new_node)) return;
    }
  }

  void remove(int data) {
      Node *curr = head->next;

      while (curr != tail && curr->data < data) {
        curr = curr->next;
      }

      if (curr == tail) return;
      if (curr->data != data) return;

      delete_node(curr);
  }

  int count(int val) {
    int n_val = 0;
    auto curr = head->next;

    while (curr != tail) {
      if (curr->data == val) n_val++;
      curr = curr->next;
    }
    return n_val;
  }

  void print_all() {
    auto curr = head->next;

    while (curr != tail) {
      std::cout << curr->data << " ";
      curr = curr->next;
    }
    std::cout << std::endl;
  }
};

}  // namespace lockfree_mcas

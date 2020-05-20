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

  void insert_before(Node *next, Node *node) {
    if (next == head) {
      return insert_after(next, node);
    }

    while (true) {
      Node *prev = next->prev;

      node->next = next;
      node->prev = prev;
      if (qcas((uint64_t *)&node->prev->next, (uint64_t)node->next, (uint64_t)node,
               (uint64_t *)&next->prev, (uint64_t)node->prev, (uint64_t)node,
               (uint64_t *)&node->next, (uint64_t)node->next, (uint64_t)next,
               (uint64_t *)&node->prev, (uint64_t)node->prev, (uint64_t)prev
               )
          ) {
        return;
      }
    }
  }

  void insert_after(Node *prev, Node *node) {
    if (prev == tail) {
      return insert_before(prev, node);
    }

    while (true) {
      Node *next = prev->next;

      node->prev = prev;
      node->next = next;

      if (dcas((uint64_t *)&prev->next, (uint64_t)node->next, (uint64_t)node,
               (uint64_t *)&node->next->prev, (uint64_t)node->prev,
               (uint64_t)node)) {
        return;
      }
    }
  }

  void delete_node(Node *node) {

    while (true) {
      if (node == head || node == tail) {
        break;
      }
      Node *prev = node->prev;
      Node *next = node->next;
      if (prev->next != node || next->prev != node) {
        continue;
      }

      if(dcas((uint64_t*)&prev->next, (uint64_t)node, (uint64_t)next,
              (uint64_t*)&next->prev, (uint64_t)node, (uint64_t)prev)) {
        return;
      }
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
      auto parent = head;
      auto curr = head->next;

      while (curr != tail && curr->data < data) {
        parent = curr;
        curr = curr->next;
      }
      insert_before(curr, new_node);
      return;
    }
  }

  void remove(int data) {
    // while (true) {
      Node *curr = head->next;

      while (curr != tail && curr->data < data) {
        curr = curr->next;
      }

      if (curr == tail) return;
      if (curr->data != data) return;

      return delete_node(curr);
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

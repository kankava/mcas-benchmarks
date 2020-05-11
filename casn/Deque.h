//
// Lock-free Deque implementation using Snark Algorithm
// as Described in Doherty et al. 2004 DCAS is not a silver bullet for
// nonblocking algorithms.
//

#pragma once

#include "mcas.h"
#include <mutex>

namespace lockfree_mcas {

class Deque {
 private:
  struct Node {
    int data;
    Node *L;
    Node *R;
    Node() = default;
  };

  Node *LeftHat;   // head
  Node *RightHat;  // tail
  Node *dummy;

 public:
  Deque() {
    dummy = new Node();
    dummy->L = dummy;
    dummy->R = dummy;
    LeftHat = dummy;
    RightHat = dummy;
  }

  ~Deque() {
    while (true) {
      int val = pop_back();
      if (val == -1) break;
    }
    delete dummy;
  }

  // push_left
  void push_front(int const& data) {
    Node *new_node = new Node();
    new_node->L = dummy;
    new_node->data = data;

    while (true) {
      Node* lh = reinterpret_cast<Node *> (MCAS_READ(&LeftHat));
      Node* lhL = reinterpret_cast<Node *>(MCAS_READ(&lh->L));
      if (lhL == lh) {
        new_node->R = dummy;
        Node* rh = reinterpret_cast<Node *>(MCAS_READ(&RightHat));
        if (dcas(reinterpret_cast<uint64_t *>(&LeftHat), reinterpret_cast<uint64_t>(lh), reinterpret_cast<uint64_t>(new_node),
                 reinterpret_cast<uint64_t *>(&RightHat), reinterpret_cast<uint64_t>(rh), reinterpret_cast<uint64_t>(new_node))) return;
      } else {
        new_node->R = reinterpret_cast<Node *>(MCAS_READ(&lh));
        if (dcas(reinterpret_cast<uint64_t *>(&LeftHat), reinterpret_cast<uint64_t>(lh), reinterpret_cast<uint64_t>(new_node),
                 reinterpret_cast<uint64_t *>(&lh->L), reinterpret_cast<uint64_t>(lhL), reinterpret_cast<uint64_t>(new_node))) return;
      }
    }
  }

  // push_right
  void push_back(int const& data) {
    Node *new_node = new Node();
    new_node->R = dummy;
    new_node->data = data;

    while (true) {
      Node* rh = reinterpret_cast<Node *>(MCAS_READ(&RightHat));
      Node* rhR = reinterpret_cast<Node *>(MCAS_READ(&rh->R));
      if (rhR == rh) {
        new_node->L = dummy;
        Node* lh = reinterpret_cast<Node *>(MCAS_READ(&LeftHat));
        if (dcas(reinterpret_cast<uint64_t *>(&RightHat), reinterpret_cast<uint64_t>(rh), reinterpret_cast<uint64_t>(new_node),
                 reinterpret_cast<uint64_t *>(&LeftHat), reinterpret_cast<uint64_t>(lh), reinterpret_cast<uint64_t>(new_node))) return;
      } else {
        new_node->L = reinterpret_cast<Node *>(MCAS_READ(&rh));
        if (dcas(reinterpret_cast<uint64_t *>(&RightHat), reinterpret_cast<uint64_t>(rh), reinterpret_cast<uint64_t>(new_node),
                 reinterpret_cast<uint64_t *>(&rh->R), reinterpret_cast<uint64_t>(rhR), reinterpret_cast<uint64_t>(new_node))) return;
      }
    }
  }

  // pop_left
  int pop_front() {
    while (true) {
      Node* lh = reinterpret_cast<Node *>(MCAS_READ(&LeftHat));
      Node* lhL = reinterpret_cast<Node *>(MCAS_READ(&(lh->L)));
      Node* lhR = reinterpret_cast<Node *>(MCAS_READ(&(lh->R)));

      if (lhL == lh) {
        if (reinterpret_cast<Node *>(MCAS_READ(reinterpret_cast<uint64_t *>(&LeftHat))) == lh) return -1;
      } else {
        if (tcas(reinterpret_cast<uint64_t *>(&LeftHat), reinterpret_cast<uint64_t>(lh), reinterpret_cast<uint64_t>(lhR),
                 reinterpret_cast<uint64_t *>(&lh->R), reinterpret_cast<uint64_t>(lhR), reinterpret_cast<uint64_t>(lh),
                 reinterpret_cast<uint64_t *>(&lh->L), reinterpret_cast<uint64_t>(lhL), reinterpret_cast<uint64_t>(lh))) {
          int result = lh->data;
          return result;
        }
      }
    }
  }

  // pop_right
  int pop_back() {
    while (true) {
      Node* rh = reinterpret_cast<Node *>(MCAS_READ(&RightHat));
      Node* rhL = reinterpret_cast<Node *>(MCAS_READ(&(rh->L)));
      Node* rhR = reinterpret_cast<Node *>(MCAS_READ(&(rh->R)));

      if (rhR == rh) {
        if (reinterpret_cast<Node *>(MCAS_READ(&RightHat)) == rh) return -1;
      } else {
        if (tcas(reinterpret_cast<uint64_t *>(&RightHat), reinterpret_cast<uint64_t>(rh), reinterpret_cast<uint64_t>(rhL),
                 reinterpret_cast<uint64_t *>(&rh->L), reinterpret_cast<uint64_t>(rhL), reinterpret_cast<uint64_t>(rh),
                 reinterpret_cast<uint64_t *>(&rh->R), reinterpret_cast<uint64_t>(rhR), reinterpret_cast<uint64_t>(rh))) {
          int result = rh->data;
          return result;
        }
      }
    }
  }

  void pop_and_print_all() {
    while (true) {
      int val = pop_front();
      if (val == -1) break;

      std::cout << val << " ";
    }
    std::cout << std::endl;
  }

  void print_all() {
    auto curr = LeftHat;
    auto lhL = curr->L;

    if (lhL == curr) return;

    while (curr != RightHat) {
      std::cout << curr->data << " ";
      curr = curr->R;
    }
    std::cout << curr->data << std::endl;
  }
};

}  // namespace lockfree_mcas

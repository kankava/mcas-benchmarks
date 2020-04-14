//
// Lock-free Deque implementation using Snark Algorithm
// as Described in Doherty et al. 2004 DCAS is not a silver bullet for
// nonblocking algorithms.
//

#pragma once

#include <memory>
#include <mutex>
#include "../mcas/mcas.h"

namespace lockfree_mcas {

template <typename T>
class Deque {
 private:
  struct Node {
    std::shared_ptr<T> data;
    std::shared_ptr<Node> L;
    std::shared_ptr<Node> R;
    Node() = default;
  };

  std::shared_ptr<Node> LeftHat;   // head
  std::shared_ptr<Node> RightHat;  // tail
  std::shared_ptr<Node> dummy;
  std::mutex cas_lock = {};

 public:
  Deque() {
    dummy = std::make_shared<Node>();
    dummy->L = dummy;
    dummy->R = dummy;
    LeftHat = dummy;
    RightHat = dummy;
  }

  // push_left
  void push_front(T const& data) {
    std::shared_ptr<Node> const new_node = std::make_shared<Node>();
    new_node->L = dummy;
    new_node->data = std::make_shared<T>(data);

    while (true) {
      auto lh = LeftHat;
      auto lhL = lh->L;
      if (lhL == lh) {
        new_node->R = dummy;
        auto rh = RightHat;
        {
          std::lock_guard<std::mutex> lock(cas_lock);
          if (DCAS(&LeftHat, &RightHat, lh, rh, new_node, new_node)) return;
        }
      } else {
        new_node->R = lh;
        {
          std::lock_guard<std::mutex> lock(cas_lock);
          if (DCAS(&LeftHat, &lh->L, lh, lhL, new_node, new_node)) return;
        }
      }
    }
  }

  // push_right
  void push_back(T const& data) {
    std::shared_ptr<Node> const new_node = std::make_shared<Node>();
    new_node->R = dummy;
    new_node->data = std::make_shared<T>(data);

    while (true) {
      auto rh = RightHat;
      auto rhR = rh->R;
      if (rhR == rh) {
        new_node->L = dummy;
        auto lh = LeftHat;
        {
          std::lock_guard<std::mutex> lock(cas_lock);
          if (DCAS(&RightHat, &LeftHat, rh, lh, new_node, new_node)) return;
        }
      } else {
        new_node->L = rh;
        {
          std::lock_guard<std::mutex> lock(cas_lock);
          if (DCAS(&RightHat, &rh->R, rh, rhR, new_node, new_node)) return;
        }
      }
    }
  }

  /*
  // buggy
  std::shared_ptr<T> pop_right() {
    while (true) {
      auto rh = RightHat;
      auto lh = LeftHat;
      if (rh->R == rh) return nullptr;
      if (rh == lh) {
        if (CAS(&RightHat, &LeftHat, rh, lh, Dummy, Dummy)) return rh->data;
      } else {
        auto rhL = rh->L;
        if (CAS(&RightHat, &rh->L, rh, rhL, rhL, rh)) {
          auto result = rh->data;
          rh->R = Dummy;
          return result;
        }
      }
    }
  }
  */

  // pop_left
  std::shared_ptr<T> pop_front() {
    while (true) {
      auto lh = LeftHat;
      auto lhL = lh->L;
      auto lhR = lh->R;

      if (lhL == lh) {
        if (LeftHat == lh) return nullptr;
      } else {
        {
          std::lock_guard<std::mutex> lock(cas_lock);
          if (CAS3(&LeftHat, &lh->R, &lh->L, lh, lhR, lhL, lhR, lh, lh))
            return lh->data;
        }
      }
    }
  }

  // pop_right
  std::shared_ptr<T> pop_back() {
    while (true) {
      auto rh = RightHat;
      auto rhL = rh->L;
      auto rhR = rh->R;

      if (rhR == rh) {
        if (RightHat == rh) return nullptr;
      } else {
        {
          std::lock_guard<std::mutex> lock(cas_lock);
          if (CAS3(&RightHat, &rh->L, &rh->R, rh, rhL, rhR, rhL, rh, rh))
            return rh->data;
        }
      }
    }
  }
};

}  // namespace lockfree_mcas

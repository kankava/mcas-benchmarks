// Original code from Patel et al. 2017 paper
// A hardware implementation of the MCAS synchronization primitive

#pragma once

#include <memory>
#include <mutex>
#include "../mcas/mcas.h"

namespace lockfree_mcas {

template <typename T>
class BinarySearchTree {
 private:
  struct Node {
    std::shared_ptr<T> value;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    Node() = default;
  };

  std::shared_ptr<Node> root;
  std::shared_ptr<Node> dummy;
  std::mutex cas_lock = {};

  typedef enum {
    LEFT,
    RIGHT,
  } node_type;

 public:
  BinarySearchTree() {
    // root = std::make_shared<Node>();
    dummy = std::make_shared<Node>(); // for CAS with nullptr, TODO: fix
  }

  void insert(const T& value) {
    std::shared_ptr<Node> const new_node = std::make_shared<Node>();
    new_node->value = std::make_shared<T>(value);

    if (!root) {
      {
        std::lock_guard<std::mutex> lock(cas_lock);
        if (CAS(&root, dummy->left, new_node)) return;
      }
    }

    std::shared_ptr<Node> curr = root;
    std::shared_ptr<Node> prev = nullptr;
    node_type type = LEFT;
    while (true) {
      while (curr) {
        prev = curr;
        if (value < *curr->value) {
          curr = curr->left;
          type = LEFT;
        } else {
          curr = curr->right;
          type = RIGHT;
        }
      }
      if (type == LEFT) {
        auto last = prev->left;
        if (CAS(&prev->left, last, new_node)) return;
      } else {
        auto last = prev->right;
        if (CAS(&prev->right, last, new_node)) return;
      }
    }
  }

  void remove(T value) {
    std::shared_ptr<Node> curr = root;
    std::shared_ptr<Node> prev = nullptr;
    node_type type = LEFT;
    while (curr) {
      if (*curr->value == value) {
        if (!curr->left && !curr->right) {  // node to be removed has no children’s
          if (curr != root && prev) {      // delete leaf node
            if (type == LEFT) {
              while (true) {
                auto last = prev;
                auto present = curr;
                if (DCAS(&curr, &prev->left,
                         present, present,
                         dummy->left, dummy->left)) return;
              }
            } else {
              while (true) {
                auto last = prev;
                auto present = curr;
                if (DCAS(&curr, &prev->right,
                         present, present,
                         dummy->right, dummy->right)) return;
              }
            }
          } else {
            auto last = root;
            //auto temp = dummy->left;
            if (CAS(&root, last, dummy->left)) return;
          }  // deleted node is root
        } else if (curr->left &&
                   curr->right) {  // node to be removed has two children’s
          curr->value = get_min(curr->right);  // find minimum value from right subtree
          value = *curr->value;
          prev = curr;
          curr = curr->right;  // continue from right subtree delete min node
          type = RIGHT;
          continue;
        } else {              // node to be removed has one children
          if (curr == root) {  // root with one child
            if (root->left) {
              auto last = root;
              auto temp = root->left;
              if (CAS(&root, last, temp)) return;
            } else {
              auto last = root;
              auto temp = root->right;
              if (CAS(&root, last, temp)) return;
            }
          } else {  // subtree with one child
            if (type == LEFT) {
              if (curr->left) {
                while (true) {
                  auto present = curr;
                  auto temp = dummy->left;
                  if (DCAS(&prev->left, &curr,
                           present, present,
                           present->left,
                           temp)) return;
                }
              } else {
                auto present = curr;
                auto temp = dummy->left;
                if (DCAS(&prev->right, &curr,
                         present, present,
                         present->right, temp)) return;
              }
            } else {
              if (curr->left) {
                auto last = curr->left;
                auto present = curr;
                auto temp = dummy->left;
                if (DCAS(&prev->left, &curr,
                         present, present,
                         last, temp)) return;
              } else {
                auto last = curr->right;
                auto present = curr;
                auto temp = dummy->left;
                if(DCAS(&prev->right, &curr,
                        present, present,
                        last, temp)) return;
              }
            }
          }
        }
      }
      prev = curr;
      if (value < *curr->value) {
        curr = curr->left;
        type = LEFT;
      } else {
        curr = curr->right;
        type = RIGHT;
      }
    }
  }

  std::shared_ptr<T> get_min() {
    auto curr = root;
    auto min = root ? root->value : nullptr;

    while (curr) {
      if (*curr->value < *min) min = curr->value;
      if (curr->left) {
        curr = curr->left;
      } else if (curr->right) {
        curr = curr->right;
      } else
        curr = nullptr;
    }
    return min;
  }

  std::shared_ptr<T> get_min(std::shared_ptr<Node> _root) {
    auto curr = _root;
    auto min = _root ? _root->value : nullptr;

    while (curr) {
      if (*curr->value < *min) min = curr->value;
      if (curr->left) {
        curr = curr->left;
      } else if (curr->right) {
        curr = curr->right;
      } else
        curr = nullptr;
    }
    return min;
  }

  std::shared_ptr<T> get_max() {
    auto curr = root;
    auto max = root ? root->value : nullptr;

    while (curr) {
      if (*curr->value > *max) max = curr->value;
      if (curr->right) {
        curr = curr->right;
      } else if (curr->left) {
        curr = curr->left;
      } else
        curr = nullptr;
    }
    return max;
  }
};

}  // namespace lockfree_mcas

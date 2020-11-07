// Original code from Patel et al. 2017 paper
// A hardware implementation of the MCAS synchronization primitive

#pragma once

#include <climits>
#include <mutex>
#include "../mcas/mcas.h"

namespace lockbased {

class BinarySearchTree {
 private:
  struct Node {
    int value;
    Node* left;
    Node* right;
    Node() = default;
  };

  Node* root;
  int sentinel_min = INT_MIN;
  int sentinel_max = INT_MAX;
  std::mutex bst_lock = {};

  typedef enum {
    LEFT,
    RIGHT,
  } node_type;

 public:
  BinarySearchTree() : root(nullptr) {};

  void insert(const int value) {
    Node *new_node = new Node();
    new_node->value = value;
    {
      std::lock_guard<std::mutex> lock(bst_lock);

      if (!root) {
	root = new_node;
	return;
      }

      Node *curr = root;
      Node *prev = nullptr;
      node_type type = LEFT;

      while (curr) {
	prev = curr;
	if (value < curr->value) {
	  curr = curr->left;
	  type = LEFT;
	} else {
	  curr = curr->right;
	  type = RIGHT;
	}

	if (type == LEFT) {
	  prev->left = new_node;
	} else {
	  prev->right = new_node;
	}
      }
    }
  }

  void remove(int value) {
    std::lock_guard<std::mutex> lock(bst_lock);
    Node *curr = root;
    Node *prev = nullptr;
    node_type type = LEFT;
    while (curr) {
      if (curr->value == value) {
        if (!curr->left && !curr->right) { // node to be removed has no children’s
          if (curr != root && prev) { // delete leaf node
            if (type == LEFT)
            prev->left = nullptr;
            else
            prev->right = nullptr;
          } else
            root = nullptr; // deleted node is root
        } else if (curr->left && curr->right) { // node to be removed has two children’s
          curr->value = get_min_UNSAFE(curr->right); // find minimum value from right subtree
          value = curr->value;
          prev = curr;
          curr = curr->right; // continue from right subtree delete min node
          type = RIGHT;
          continue;
        } else { // node to be removed has one children
          if (curr == root) { // root with one child
            root = root->left ? root->left : root->right;
          } else { // subtree with one child
            if (type == LEFT)
            prev->left = curr->left ? curr->left : curr->right;
            else
            prev->right = curr->left ? curr->left : curr->right;
          }
        }
      }
      prev = curr;
      if (value < curr->value) {
        curr = curr->left;
        type = LEFT;
      } else {
        curr = curr->right;
        type = RIGHT;
      }
    }
  }

  int get_min() {
    std::lock_guard<std::mutex> lock(bst_lock);
    int min = get_min_UNSAFE(root);
    return min;
  }

  int get_min(Node *_root) {
    std::lock_guard<std::mutex> lock(bst_lock);
    int min = get_min_UNSAFE(_root);
    return min;
  }

  int get_max() {
    std::lock_guard<std::mutex> lock(bst_lock);
    int max = get_max_UNSAFE();
    return max;
  }

 private:

  int get_min_UNSAFE(Node *_root) {
    auto curr = _root;
    auto min = _root ? _root->value : sentinel_max;

    while (curr) {
      if (curr->value < min) min = curr->value;
      if (curr->left) {
        curr = curr->left;
      } else if (curr->right) {
        curr = curr->right;
      } else
        curr = nullptr;
    }
    return min;
  }

  int get_max_UNSAFE() {
    auto curr = root;
    auto max = root ? root->value : sentinel_min;

    while (curr) {
      if (curr->value > max) max = curr->value;
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

}  // namespace lockbased

#pragma once

#include <memory>
#include <mutex>

#include "SortedList.h"

#define TABLE_SIZE 10000

namespace lockbased {

template <typename Key, typename Value>
class HashMap {
 private:
  struct Node {
    std::shared_ptr<Key> key;
    std::shared_ptr<Value> value;
    std::shared_ptr<Node> next;
    std::shared_ptr<Node> prev;
    Node() = default;
  };

  std::shared_ptr<Node> bucket_heads[TABLE_SIZE];
  std::shared_ptr<Node> bucket_tails[TABLE_SIZE];
  std::mutex hm_lock = {};

 public:
  HashMap() {
    for (int i = 0; i < TABLE_SIZE; i++) {
      bucket_heads[i] = std::make_shared<Node>();
      bucket_tails[i] = std::make_shared<Node>();
      bucket_heads[i]->next = bucket_tails[i];
      bucket_tails[i]->prev = bucket_heads[i];
    }
  }

  void insert_or_assign(Key const& key, Value const& value) {
    std::lock_guard<std::mutex> lock(hm_lock);
    unsigned long index = std::hash<Key>{}(key) % TABLE_SIZE;
    
    std::shared_ptr<Node> parent = bucket_heads[index];
    std::shared_ptr<Node> curr = bucket_heads[index]->next;
    std::shared_ptr<Node> tail = bucket_tails[index];

    while (curr != tail && *curr->key != key) {
        parent = curr;
        curr = curr->next;
    }

    if (curr == tail) {
      std::shared_ptr<Node> const new_node = std::make_shared<Node>();
      new_node->key = std::make_shared<Key>(key);
      new_node->value = std::make_shared<Value>(value);

      new_node->next = curr;
      new_node->prev = parent;
      
      parent->next = new_node;
      curr->prev = new_node;
    } else {
        *curr->value = value;
    }
  }

  bool contains(Key key) {
    std::lock_guard<std::mutex> lock(hm_lock);
    unsigned long index = std::hash<Key>{}(key) % TABLE_SIZE;
    std::shared_ptr<Node> curr = bucket_heads[index]->next;
    std::shared_ptr<Node> tail = bucket_tails[index];

    while (curr != tail) {
      if (*curr->key == key) {
        return true;
      } else {
        curr = curr->next;
      }
    }

    return false;
  }

  void remove(Key const& key) {
    std::lock_guard<std::mutex> lock(hm_lock);
    unsigned long index = std::hash<Key>{}(key) % TABLE_SIZE;

    std::shared_ptr<Node> curr = bucket_heads[index]->next;
    std::shared_ptr<Node> tail = bucket_tails[index];

    while (curr != tail && *curr->key != key) {
        curr = curr->next;
    }

    if (curr == tail) return;

    curr->next->prev = curr->prev;
    curr->prev->next = curr->next;

    return;
  }

  std::shared_ptr<Value> find(Key key) {
    std::lock_guard<std::mutex> lock(hm_lock);
    unsigned long index = std::hash<Key>{}(key) % TABLE_SIZE;

    std::shared_ptr<Node> curr = bucket_heads[index]->next;
    std::shared_ptr<Node> tail = bucket_tails[index];

     while (curr != tail) {
        if (*curr->key == key) return curr->value;
    }

    return nullptr;
  }
  
};

}  // namespace lockbased

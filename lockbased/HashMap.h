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

  std::shared_ptr<Node> buckets[TABLE_SIZE];
  std::mutex hm_lock = {};

 public:
  HashMap() : buckets() {}

  void insert_or_assign(Key const& key, Value const& value) {
    std::lock_guard<std::mutex> lock(hm_lock);
    unsigned long index = std::hash<Key>{}(key) % TABLE_SIZE;
    
    std::shared_ptr<Node> prev;
    std::shared_ptr<Node> curr = buckets[index];

    while (curr != nullptr && *curr->key != key) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == nullptr) {
        std::shared_ptr<Node> const new_node = std::make_shared<Node>();
        new_node->key = std::make_shared<Key>(key);
        new_node->value = std::make_shared<Value>(value);

        if (prev == nullptr) {
            buckets[index] = new_node;
        } else {
            prev->next = new_node;
            new_node->prev = prev;
        }
    } else {
        *curr->value = value;
    }
  }

  bool contains(Key key) {
    std::lock_guard<std::mutex> lock(hm_lock);
    unsigned long index = std::hash<Key>{}(key) % TABLE_SIZE;
    std::shared_ptr<Node> curr = buckets[index];

    while (curr != nullptr) {
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

    std::shared_ptr<Node> prev;
    std::shared_ptr<Node> curr = buckets[index];

    while (curr != nullptr && *curr->key != key) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == nullptr) return;

    if (prev == nullptr) {
      if (curr->next) {
        curr->next->prev = nullptr;
      }
      buckets[index] = curr->next;
    } else {
        curr->next->prev = curr->prev;
        curr->prev->next = curr->next;
    }

    return;
  }

  std::shared_ptr<Value> find(Key key) {
    std::lock_guard<std::mutex> lock(hm_lock);
    unsigned long index = std::hash<Key>{}(key) % TABLE_SIZE;

    std::shared_ptr<Node> curr = buckets[index];

     while (curr != nullptr) {
        if (*curr->key == key) return curr->value;
    }

    return nullptr;
  }
  
};

}  // namespace lockbased

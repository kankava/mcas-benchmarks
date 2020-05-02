#pragma once

#include <memory>
#include <mutex>

#define TABLE_SIZE 10000

namespace lockfree_mcas {

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
  std::shared_ptr<Node> dummy;
  std::mutex cas_lock = {};

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
      new_node->next = dummy;
      new_node->prev = dummy;

      new_node->next = curr;
      new_node->prev = parent;
      {
        std::lock_guard<std::mutex> lock(cas_lock);
        if (CAS4(&parent->next, &curr->prev, &new_node->next, &new_node->prev,
                  curr, parent, dummy, dummy,
                  new_node, new_node, curr, parent))
          return;
      }
    } else {
      auto new_value = std::make_shared<Value>(value);
      {
        std::lock_guard<std::mutex> lock(cas_lock);
        if (CAS(&curr->value, curr->value, new_value)) return;
      }
    }
  }

  bool contains(Key key) {
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
    unsigned long index = std::hash<Key>{}(key) % TABLE_SIZE;

    std::shared_ptr<Node> curr = bucket_heads[index]->next;
    std::shared_ptr<Node> tail = bucket_tails[index];

    while (true) {
      while (curr != tail && *curr->key != key) {
          curr = curr->next;
      }

      if (curr == tail) return;

      curr->next->prev = curr->prev;
      curr->prev->next = curr->next;

      std::shared_ptr<Node> c_n_prev = curr->next->prev;
      std::shared_ptr<Node> c_p_next = curr->prev->next;

      {
        std::lock_guard<std::mutex> lock(cas_lock);
        if (DCAS(&curr->next->prev, &curr->prev->next,
                  c_n_prev, c_p_next,
                  curr->prev, curr->next))
          return;
      }
    }
  }

  std::shared_ptr<Value> find(Key key) {
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

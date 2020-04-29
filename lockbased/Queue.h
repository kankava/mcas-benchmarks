#pragma once

#include "Deque.h"

namespace lockbased {

template <typename T>
class Queue : Deque<T> {
 public:
  void push(T const& data) { return Deque<T>::push_back(data); }
  std::shared_ptr<T> pop() { return Deque<T>::pop_front(); }
};

}  // namespace lockbased

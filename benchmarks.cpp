//
// Created by kankava on 2020-04-13.
//

#include "benchmarks.h"
#include "benchmark.h"
#include "lockfree-mcas/Deque.h"
#include "lockfree-mcas/Stack.h"
#include "lockfree-mcas/Queue.h"
#include "lockfree-mcas/SortedList.h"

static const int DATA_VALUE_RANGE_MIN = 0;
static const int DATA_VALUE_RANGE_MAX = 256;
static const int DATA_PREFILL = 512;

void benchmark_deque() {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  // benchmark deque
  {
    lockfree_mcas::Deque<int> deque;

    // prefill list with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      deque.push_back(uniform_dist(engine));
    }

    benchmark(6, u8"Lock-Free Deque Update", [&deque](int random) {
      auto choice1 =
          (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      auto choice2 =
          (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      if (choice1 == 0) {
        if (choice2 == 0) {
          deque.push_back(random % DATA_VALUE_RANGE_MAX);
        } else {
          deque.push_front(random % DATA_VALUE_RANGE_MAX);
        }
      } else {
        if (choice2 == 0) {
          deque.pop_back();
        } else {
          deque.pop_front();
        }
      }
    });
  }
}

void benchmark_stack() {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  // benchmark deque
  {
    lockfree_mcas::Stack<int> stack;

    // prefill list with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      stack.push(uniform_dist(engine));
    }

    benchmark(6, u8"Lock-Free Stack Update", [&stack](int random) {
      auto choice =
          (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      if (choice == 0) {
        stack.push(random % DATA_VALUE_RANGE_MAX);;
      } else {
        stack.pop();
      }
    });
  }
}

void benchmark_queue() {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  // benchmark deque
  {
    lockfree_mcas::Queue<int> queue;

    // prefill list with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      queue.push(uniform_dist(engine));
    }

    benchmark(6, u8"Lock-Free Queue Update", [&queue](int random) {
      auto choice =
          (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      if (choice == 0) {
        queue.push(random % DATA_VALUE_RANGE_MAX);;
      } else {
        queue.pop();
      }
    });
  }
}


template<typename List>
void read(List& l, int random) {
  /* read operations: 100% count */
  l.count(random % DATA_VALUE_RANGE_MAX);
}

template<typename List>
void update(List& l, int random) {
  /* update operations: 50% insert, 50% remove */
  auto choice = (random % (2*DATA_VALUE_RANGE_MAX))/DATA_VALUE_RANGE_MAX;
  // if(choice == 0) {
    l.insert(random % DATA_VALUE_RANGE_MAX);
  // } else {
  //   l.remove(random % DATA_VALUE_RANGE_MAX);
  // }
}

template<typename List>
void mixed(List& l, int random) {
  /* mixed operations: 6.25% update, 93.75% count */
  auto choice = (random % (32*DATA_VALUE_RANGE_MAX))/DATA_VALUE_RANGE_MAX;
  if(choice == 0) {
    l.insert(random % DATA_VALUE_RANGE_MAX);
  } else if(choice == 1) {
    l.remove(random % DATA_VALUE_RANGE_MAX);
  } else {
    l.count(random % DATA_VALUE_RANGE_MAX);
  }
}

void benchmark_sorted_list() {
/* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN, DATA_VALUE_RANGE_MAX);

  /* example use of benchmarking */
  {
    lockfree_mcas::SortedList<int> l1;
    /* prefill list with 1024 elements */
    for(int i = 0; i < DATA_PREFILL; i++) {
      l1.insert(uniform_dist(engine));
    }
    // benchmark(6, u8"lock-free read", [&l1](int random){
    //   read(l1, random);
    // });
    benchmark(6, u8"lock-free update", [&l1](int random){
      update(l1, random);
    });
  }
  // {
  //   /* start with fresh list: update test left list in random size */
  //   lockfree_mcas::SortedList<int> l1;
  //   /* prefill list with 1024 elements */
  //   for(int i = 0; i < DATA_PREFILL; i++) {
  //     l1.insert(uniform_dist(engine));
  //   }
  //   benchmark(6, u8"lock-free mixed", [&l1](int random){
  //     mixed(l1, random);
  //   });
  // }
}

void run_all() {
  // benchmark_deque();
  // benchmark_stack();
  // benchmark_queue();
  benchmark_sorted_list();
}
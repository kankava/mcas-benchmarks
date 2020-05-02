#include "benchmarks.h"
#include "benchmark.h"
#include "configuration.h"

#include "lockbased/Deque.h"
#include "lockbased/Queue.h"
#include "lockbased/Stack.h"
#include "lockbased/SortedList.h"
#include "lockbased/HashMap.h"

#include "lockfree-mcas/Deque.h"
#include "lockfree-mcas/Queue.h"
#include "lockfree-mcas/Stack.h"
#include "lockfree-mcas/SortedList.h"
#include "lockfree-mcas/HashMap.h"

static const int DATA_VALUE_RANGE_MIN = 0;
static const int DATA_VALUE_RANGE_MAX = 256;
static const int DATA_PREFILL = 512;

namespace lockbased {

void benchmark_deque() {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  {
    lockbased::Deque<int> deque;

    // prefill deque with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      deque.push_back(uniform_dist(engine));
    }

    benchmark(6, u8"Locking Deque Update", [&deque](int random) {
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

  {
    lockbased::Stack<int> stack;

    // prefill stack with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      stack.push(uniform_dist(engine));
    }

    benchmark(6, u8"Locking Stack Update", [&stack](int random) {
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

  {
    lockbased::Queue<int> queue;

    // prefill queue with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      queue.push(uniform_dist(engine));
    }

    benchmark(6, u8"Locking Queue Update", [&queue](int random) {
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
  if(choice == 0) {
    l.insert(random % DATA_VALUE_RANGE_MAX);
  } else {
    l.remove(random % DATA_VALUE_RANGE_MAX);
  }
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

  {
    lockbased::SortedList<int> l1;
    /* prefill list with 1024 elements */
    for(int i = 0; i < DATA_PREFILL; i++) {
      l1.insert(uniform_dist(engine));
    }
    benchmark(6, u8"locking list read", [&l1](int random){
      read(l1, random);
    });
    benchmark(6, u8"locking list update", [&l1](int random){
      update(l1, random);
    });
  }

  {
    lockbased::SortedList<int> l1;
    /* prefill list with 1024 elements */
    for(int i = 0; i < DATA_PREFILL; i++) {
      l1.insert(uniform_dist(engine));
    }
    benchmark(6, u8"locking list mixed", [&l1](int random){
      mixed(l1, random);
    });
  }
}

template<typename HashMap>
void hm_lookup(HashMap& map, int random) {
  /* read operations: 100% count */
  map.contains(random % DATA_VALUE_RANGE_MAX);
}

template<typename HashMap>
void hm_update(HashMap& map, int random) {
  /* update operations: 50% insert, 50% remove */
  auto choice = (random % (2*DATA_VALUE_RANGE_MAX))/DATA_VALUE_RANGE_MAX;
  if(choice == 0) {
    map.insert_or_assign(random % DATA_VALUE_RANGE_MAX, random % DATA_VALUE_RANGE_MAX);
  } else {
    map.remove(random % DATA_VALUE_RANGE_MAX);
  }
}

template<typename HashMap>
void hm_mixed(HashMap& map, int random) {
  /* mixed operations: 6.25% update, 93.75% count */
  auto choice = (random % (32*DATA_VALUE_RANGE_MAX))/DATA_VALUE_RANGE_MAX;
  if(choice == 0) {
    map.insert_or_assign(random % DATA_VALUE_RANGE_MAX, random % DATA_VALUE_RANGE_MAX);
  } else if(choice == 1) {
    map.remove(random % DATA_VALUE_RANGE_MAX);
  } else {
    map.contains(random % DATA_VALUE_RANGE_MAX);
  }
}

void benchmark_hashmap() {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN, DATA_VALUE_RANGE_MAX);

  {
    lockbased::HashMap<int, int> l1;
    /* prefill list with 1024 elements */
    for(int i = 0; i < DATA_PREFILL; i++) {
      l1.insert_or_assign(uniform_dist(engine), uniform_dist(engine));
    }
    benchmark(6, u8"locking hashmap lookup", [&l1](int random){
      hm_lookup(l1, random);
    });
    benchmark(6, u8"locking hashmap update", [&l1](int random){
      hm_update(l1, random);
    });
  }

  {
    lockbased::HashMap<int, int> l1;
    /* prefill list with 1024 elements */
    for(int i = 0; i < DATA_PREFILL; i++) {
      l1.insert_or_assign(uniform_dist(engine), uniform_dist(engine));
    }
    benchmark(6, u8"lock-based hashmap mixed", [&l1](int random){
      hm_mixed(l1, random);
    });
  }
}

}  // namespace lockbased

namespace lockfree_mcas {

void benchmark_mwobject() {
  struct {
    int a;
    int b;
    int c;
    int d;
  } counters{};

  std::mutex cas_lock = {};

  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  {
    benchmark(6, u8"Lock-Free MW Object Update", [&counters, &cas_lock](int random) {
      auto choice =
          (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      if (choice == 0) {
        while (true) {
          int old_a = counters.a;
          int old_b = counters.b;
          int old_c = counters.c;
          int old_d = counters.d;

          int new_a = old_a + 1;
          int new_b = old_b + 1;
          int new_c = old_c + 1;
          int new_d = old_d + 1;

          {
            std::lock_guard<std::mutex> lock(cas_lock);
            if (CAS(&counters.a, &counters.b, &counters.c, &counters.d,
                    old_a, old_b, old_c, old_d,
                    new_a, new_b, new_c, new_d))
              break;
          }
        }
      } else {
        while (true) {
          int old_a = counters.a;
          int old_b = counters.b;
          int old_c = counters.c;
          int old_d = counters.d;

          int new_a = old_a - 1;
          int new_b = old_b - 1;
          int new_c = old_c - 1;
          int new_d = old_d - 1;

          {
            std::lock_guard<std::mutex> lock(cas_lock);
            if (CAS(&counters.a, &counters.b, &counters.c, &counters.d,
                    old_a, old_b, old_c, old_d,
                    new_a, new_b, new_c, new_d))
              break;
          }
        }
      }
    });
  }
}

void benchmark_deque() {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  {
    lockfree_mcas::Deque<int> deque;

    // prefill deque with 1024 elements
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

  {
    lockfree_mcas::Stack<int> stack;

    // prefill stack with 1024 elements
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

  {
    lockfree_mcas::Queue<int> queue;

    // prefill queue with 1024 elements
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
  if(choice == 0) {
    l.insert(random % DATA_VALUE_RANGE_MAX);
  } else {
    l.remove(random % DATA_VALUE_RANGE_MAX);
  }
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

  {
    lockfree_mcas::SortedList<int> l1;
    /* prefill list with 1024 elements */
    for(int i = 0; i < DATA_PREFILL; i++) {
      l1.insert(uniform_dist(engine));
    }
    benchmark(6, u8"lock-free read", [&l1](int random){
      read(l1, random);
    });
    benchmark(6, u8"lock-free update", [&l1](int random){
      update(l1, random);
    });
  }

  {
    lockfree_mcas::SortedList<int> l1;
    /* prefill list with 1024 elements */
    for(int i = 0; i < DATA_PREFILL; i++) {
      l1.insert(uniform_dist(engine));
    }
    benchmark(6, u8"lock-free mixed", [&l1](int random){
      mixed(l1, random);
    });
  }
}

template<typename HashMap>
void hm_lookup(HashMap& map, int random) {
  /* read operations: 100% count */
  map.contains(random % DATA_VALUE_RANGE_MAX);
}

template<typename HashMap>
void hm_update(HashMap& map, int random) {
  /* update operations: 50% insert, 50% remove */
  auto choice = (random % (2*DATA_VALUE_RANGE_MAX))/DATA_VALUE_RANGE_MAX;
  if(choice == 0) {
    map.insert_or_assign(random % DATA_VALUE_RANGE_MAX, random % DATA_VALUE_RANGE_MAX);
  } else {
    map.remove(random % DATA_VALUE_RANGE_MAX);
  }
}

template<typename HashMap>
void hm_mixed(HashMap& map, int random) {
  /* mixed operations: 6.25% update, 93.75% count */
  auto choice = (random % (32*DATA_VALUE_RANGE_MAX))/DATA_VALUE_RANGE_MAX;
  if(choice == 0) {
    map.insert_or_assign(random % DATA_VALUE_RANGE_MAX, random % DATA_VALUE_RANGE_MAX);
  } else if(choice == 1) {
    map.remove(random % DATA_VALUE_RANGE_MAX);
  } else {
    map.contains(random % DATA_VALUE_RANGE_MAX);
  }
}

void benchmark_hashmap() {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN, DATA_VALUE_RANGE_MAX);

  {
    lockfree_mcas::HashMap<int, int> l1;
    /* prefill list with 1024 elements */
    for(int i = 0; i < DATA_PREFILL; i++) {
      l1.insert_or_assign(uniform_dist(engine), uniform_dist(engine));
    }
    benchmark(6, u8"lock-free hashmap lookup", [&l1](int random){
      hm_lookup(l1, random);
    });
    benchmark(6, u8"lock-free hashmap update", [&l1](int random){
      hm_update(l1, random);
    });
  }

  {
    lockfree_mcas::HashMap<int, int> l1;
    /* prefill list with 1024 elements */
    for(int i = 0; i < DATA_PREFILL; i++) {
      l1.insert_or_assign(uniform_dist(engine), uniform_dist(engine));
    }
    benchmark(6, u8"lock-free hashmap mixed", [&l1](int random){
      hm_mixed(l1, random);
    });
  }
}

} // namespace lockfree_mcas

void run_benchmarks(const Configuration &config) {
  // TODO: run benchmarks according to config
  // switch (config.sync_type) {
  //   case Configuration::SYNC_UNDEF:
  //     break;
  //   case Configuration::LOCK:
  //     break;
  //   case Configuration::LOCKFREE:
  //     break;
  //   case Configuration::LOCKFREE_MCAS:
  //     break;
  // }

  /*  benchmark_mwobject();
    benchmark_deque();
    benchmark_stack();
    benchmark_queue();
    benchmark_sorted_list();
  */
  // lockbased::benchmark_deque();
  // lockbased::benchmark_stack();
  // lockbased::benchmark_queue();
  // lockbased::benchmark_sorted_list();
  lockbased::benchmark_hashmap();
  
  // lockfree_mcas::benchmark_sorted_list();
  lockfree_mcas::benchmark_hashmap();
}

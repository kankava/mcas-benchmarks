#include "benchmarks.h"
#include "benchmark.h"
#include "configuration.h"

#include "lockbased/Deque.h"
#include "lockbased/HashMap.h"
#include "lockbased/Queue.h"
#include "lockbased/SortedList.h"
#include "lockbased/Stack.h"

#include "lockfree-mcas/BinarySearchTree.h"
#include "lockfree-mcas/Deque.h"
#include "lockfree-mcas/HashMap.h"
#include "lockfree-mcas/Queue.h"
#include "lockfree-mcas/SortedList.h"
#include "lockfree-mcas/Stack.h"

static const int DATA_VALUE_RANGE_MIN = 0;
static const int DATA_VALUE_RANGE_MAX = 256;
static const int DATA_PREFILL = 512;

void benchmark_mwobject(const Configuration& config) {
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
    benchmark(config.n_threads, u8"Update", [&counters, &cas_lock](int random) {
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
            if (CAS(&counters.a, &counters.b, &counters.c, &counters.d, old_a,
                    old_b, old_c, old_d, new_a, new_b, new_c, new_d))
              break;
          }
        }
      }
    });
  }
}

template <typename Deque>
void benchmark_deque(Deque& deque, const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  {
    // prefill deque with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      deque.push_back(uniform_dist(engine));
    }

    benchmark(config.n_threads, u8"update", [&deque](int random) {
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

template <typename Stack>
void benchmark_stack(Stack& stack, const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  {
    // prefill stack with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      stack.push(uniform_dist(engine));
    }

    benchmark(config.n_threads, u8"update", [&stack](int random) {
      auto choice =
          (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      if (choice == 0) {
        stack.push(random % DATA_VALUE_RANGE_MAX);
        ;
      } else {
        stack.pop();
      }
    });
  }
}

template <typename Queue>
void benchmark_queue(Queue& queue, const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  {
    // prefill queue with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      queue.push(uniform_dist(engine));
    }

    benchmark(config.n_threads, u8"update", [&queue](int random) {
      auto choice =
          (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      if (choice == 0) {
        queue.push(random % DATA_VALUE_RANGE_MAX);
        ;
      } else {
        queue.pop();
      }
    });
  }
}

template <typename List>
void read(List& l, int random) {
  /* read operations: 100% count */
  l.count(random % DATA_VALUE_RANGE_MAX);
}

template <typename List>
void update(List& l, int random) {
  /* update operations: 50% insert, 50% remove */
  auto choice = (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
  if (choice == 0) {
    l.insert(random % DATA_VALUE_RANGE_MAX);
  } else {
    l.remove(random % DATA_VALUE_RANGE_MAX);
  }
}

template <typename List>
void mixed(List& l, int random) {
  /* mixed operations: 6.25% update, 93.75% count */
  auto choice = (random % (32 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
  if (choice == 0) {
    l.insert(random % DATA_VALUE_RANGE_MAX);
  } else if (choice == 1) {
    l.remove(random % DATA_VALUE_RANGE_MAX);
  } else {
    l.count(random % DATA_VALUE_RANGE_MAX);
  }
}

template <typename List>
void benchmark_sorted_list(List& list1, List& list2,
                           const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      list1.insert(uniform_dist(engine));
    }
    benchmark(config.n_threads, u8"read",
              [&list1](int random) { read(list1, random); });
    benchmark(config.n_threads, u8"update",
              [&list1](int random) { update(list1, random); });
  }

  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      list2.insert(uniform_dist(engine));
    }
    benchmark(6, u8"mixed", [&list2](int random) { mixed(list2, random); });
  }
}

template <typename HashMap>
void hm_lookup(HashMap& map, int random) {
  /* read operations: 100% count */
  map.contains(random % DATA_VALUE_RANGE_MAX);
}

template <typename HashMap>
void hm_update(HashMap& map, int random) {
  /* update operations: 50% insert, 50% remove */
  auto choice = (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
  if (choice == 0) {
    map.insert_or_assign(random % DATA_VALUE_RANGE_MAX,
                         random % DATA_VALUE_RANGE_MAX);
  } else {
    map.remove(random % DATA_VALUE_RANGE_MAX);
  }
}

template <typename HashMap>
void hm_mixed(HashMap& map, int random) {
  /* mixed operations: 6.25% update, 93.75% count */
  auto choice = (random % (32 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
  if (choice == 0) {
    map.insert_or_assign(random % DATA_VALUE_RANGE_MAX,
                         random % DATA_VALUE_RANGE_MAX);
  } else if (choice == 1) {
    map.remove(random % DATA_VALUE_RANGE_MAX);
  } else {
    map.contains(random % DATA_VALUE_RANGE_MAX);
  }
}

template <typename HashMap>
void benchmark_hashmap(HashMap& map1, HashMap& map2,
                       const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      map1.insert_or_assign(uniform_dist(engine), uniform_dist(engine));
    }
    benchmark(config.n_threads, u8"read",
              [&map1](int random) { hm_lookup(map1, random); });
    benchmark(config.n_threads, u8"update",
              [&map1](int random) { hm_update(map1, random); });
  }

  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      map2.insert_or_assign(uniform_dist(engine), uniform_dist(engine));
    }
    benchmark(config.n_threads, u8"mixed",
              [&map2](int random) { hm_mixed(map2, random); });
  }
}

template <typename BST>
void bst_lookup(BST& bst, int random) {
  /* read operations: 100% count */
  bst.get_min();
}

template <typename BST>
void bst_update(BST& bst, int random) {
  /* update operations: 50% insert, 50% remove */
  auto choice = (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
  if (choice == 0) {
    bst.insert(random % DATA_VALUE_RANGE_MAX);
  } else {
    bst.remove(random % DATA_VALUE_RANGE_MAX);
  }
}

template <typename BST>
void bst_mixed(BST& bst, int random) {
  /* mixed operations: 6.25% update, 93.75% count */
  auto choice = (random % (32 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
  if (choice == 0) {
    bst.insert(random % DATA_VALUE_RANGE_MAX);
  } else if (choice == 1) {
    bst.remove(random % DATA_VALUE_RANGE_MAX);
  } else {
    bst.get_min();
  }
}

template <typename BST>
void benchmark_bst(BST& bst1, BST& bst2, const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      bst1.insert(uniform_dist(engine));
    }
    benchmark(config.n_threads, u8"read",
              [&bst1](int random) { bst_lookup(bst1, random); });
    benchmark(config.n_threads, u8"update",
              [&bst1](int random) { bst_update(bst1, random); });
  }

  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      bst2.insert(uniform_dist(engine));
    }
    benchmark(config.n_threads, u8"mixed",
              [&bst2](int random) { bst_mixed(bst2, random); });
  }
}

void run_benchmarks(const Configuration& config) {
  switch (config.sync_type) {
    case Configuration::SyncType::LOCK: {
      switch (config.benchmarking_algorithm) {
        case Configuration::BenchmarkAlgorithm::MWOBJECT: {
        } break;
        case Configuration::BenchmarkAlgorithm::STACK: {
          std::cout << "Benchmark Locking Stack" << std::endl;
          lockbased::Stack<int> stack;
          benchmark_stack(stack, config);
        } break;
        case Configuration::BenchmarkAlgorithm::QUEUE: {
          std::cout << "Benchmark Locking Queue" << std::endl;
          lockbased::Queue<int> queue;
          benchmark_queue(queue, config);
        } break;
        case Configuration::BenchmarkAlgorithm::DEQUE: {
          std::cout << "Benchmark Locking Deque" << std::endl;
          lockbased::Deque<int> deque;
          benchmark_deque(deque, config);
        } break;
        case Configuration::BenchmarkAlgorithm::SORTEDLIST: {
          std::cout << "Benchmark Locking Sorted List" << std::endl;
          lockbased::SortedList<int> list1;
          lockbased::SortedList<int> list2;
          benchmark_sorted_list(list1, list2, config);
        } break;
        case Configuration::BenchmarkAlgorithm::HASHMAP: {
          std::cout << "Benchmark Locking HashMap" << std::endl;
          lockbased::HashMap<int, int> map1;
          lockbased::HashMap<int, int> map2;
          benchmark_hashmap(map1, map2, config);
        } break;
        case Configuration::BenchmarkAlgorithm::BST: {
          std::cout << "TODO: Benchmark Locking BST" << std::endl;
        } break;
      }
    } break;
    case Configuration::SyncType::LOCKFREE: {
      switch (config.benchmarking_algorithm) {
        case Configuration::BenchmarkAlgorithm::MWOBJECT: {
        } break;
        case Configuration::BenchmarkAlgorithm::STACK: {
        } break;
        case Configuration::BenchmarkAlgorithm::QUEUE: {
        } break;
        case Configuration::BenchmarkAlgorithm::DEQUE: {
        } break;
        case Configuration::BenchmarkAlgorithm::SORTEDLIST: {
        } break;
        case Configuration::BenchmarkAlgorithm::HASHMAP: {
        } break;
        case Configuration::BenchmarkAlgorithm::BST: {
        } break;
      }
      // TODO
      exit(0);
    } break;
    case Configuration::SyncType::LOCKFREE_MCAS: {
      switch (config.benchmarking_algorithm) {
        case Configuration::BenchmarkAlgorithm::MWOBJECT: {
          std::cout << "Benchmark Lock-Free MCAS MW Object" << std::endl;
          benchmark_mwobject(config);
        } break;
        case Configuration::BenchmarkAlgorithm::STACK: {
          std::cout << "Benchmark Lock-Free MCAS Stack" << std::endl;
          lockfree_mcas::Stack<int> stack;
          benchmark_stack(stack, config);
        } break;
        case Configuration::BenchmarkAlgorithm::QUEUE: {
          std::cout << "Benchmark Lock-Free MCAS Queue" << std::endl;
          lockfree_mcas::Queue<int> queue;
          benchmark_queue(queue, config);
        } break;
        case Configuration::BenchmarkAlgorithm::DEQUE: {
          std::cout << "Benchmark Lock-Free MCAS Deque" << std::endl;
          lockfree_mcas::Deque<int> deque;
          benchmark_deque(deque, config);
        } break;
        case Configuration::BenchmarkAlgorithm::SORTEDLIST: {
          std::cout << "Benchmark Lock-Free MCAS Sorted List" << std::endl;
          lockfree_mcas::SortedList<int> list1;
          lockfree_mcas::SortedList<int> list2;
          benchmark_sorted_list(list1, list2, config);
        } break;
        case Configuration::BenchmarkAlgorithm::HASHMAP: {
          std::cout << "Benchmark Lock-Free MCAS HashMap" << std::endl;
          lockfree_mcas::HashMap<int, int> map1;
          lockfree_mcas::HashMap<int, int> map2;
          benchmark_hashmap(map1, map2, config);
        } break;
        case Configuration::BenchmarkAlgorithm::BST: {
          std::cout << "Benchmark Lock-Free MCAS BST" << std::endl;
          lockfree_mcas::BinarySearchTree<int> bst1;
          lockfree_mcas::BinarySearchTree<int> bst2;
          benchmark_bst(bst1, bst2, config);
        } break;
      }
    } break;
  }

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
  // lockbased::benchmark_hashmap();

  // lockfree_mcas::benchmark_sorted_list();
  // lockfree_mcas::benchmark_hashmap();
  // lockfree_mcas::benchmark_bst();
}

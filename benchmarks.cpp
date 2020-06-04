#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#include "benchmarks.h"
#include "benchmark.h"
#include "configuration.h"

#include "lockbased/Deque.h"
#include "lockbased/HashMap.h"
#include "lockbased/Queue.h"
#include "lockbased/SortedList.h"
#include "lockbased/Stack.h"
#include "lockbased/BinarySearchTree.h"
#include "lockbased/array_swap.h"

#include "lockfree/BinarySearchTree.h"
#include "lockfree/SortedList.h"
#include "lockfree/HashMap.h"
#include "lockfree/Deque.h"
#include "lockfree/Stack.h"
#include "lockfree/Queue.h"

#include "lockfree-mcas/BinarySearchTree.h"
#include "lockfree-mcas/Deque.h"
#include "lockfree-mcas/HashMap.h"
#include "lockfree-mcas/Queue.h"
#include "lockfree-mcas/SortedList.h"
#include "lockfree-mcas/Stack.h"
#include "lockfree-mcas/array_swap.h"

static const int DATA_VALUE_RANGE_MIN = 0;
static const int DATA_VALUE_RANGE_MAX = 256;
static const int DATA_PREFILL = 1024;

void benchmark_mwobject(const Configuration& config) {
  struct {
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
  } counters{};
  std::mutex counters_lock;

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    benchmark(config.n_threads, config.n_ops, u8"Update", [&counters, &counters_lock](int random) {
      std::lock_guard<std::mutex> lock(counters_lock);
      counters.a++;
      counters.b++;
      counters.c++;
      counters.d++;
    });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

}

void benchmark_mcas_mwobject(const Configuration& config) {
  struct {
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
  } counters{};

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    benchmark(config.n_threads, config.n_ops, u8"Update", [&counters](int random) {
      while (true) {
        uint64_t old_a = counters.a;
        uint64_t old_b = counters.b;
        uint64_t old_c = counters.c;
        uint64_t old_d = counters.d;

        uint64_t new_a = old_a + 1;
        uint64_t new_b = old_b + 1;
        uint64_t new_c = old_c + 1;
        uint64_t new_d = old_d + 1;

        {
          if (qcas(reinterpret_cast<uint64_t*>(&counters.a), old_a, new_a,
                   reinterpret_cast<uint64_t*>(&counters.b), old_b, new_b,
                   reinterpret_cast<uint64_t*>(&counters.c), old_c, new_c,
                   reinterpret_cast<uint64_t*>(&counters.d), old_d, new_d))
            break;
        }
      }
    });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

}

void benchmark_arrayswap(const Configuration& config) {
    lockbased::ArraySwap::initialize();

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    benchmark(config.n_threads, config.n_ops, u8"swap", [](int random) {
      /* set up random number generator */
      std::random_device rd;
      std::mt19937 engine(rd());
      std::uniform_int_distribution<int> uniform_dist(0, lockbased::ArraySwap::NUM_ROWS-1);

      int index_a = uniform_dist(engine);
      int index_b = uniform_dist(engine);
      lockbased::ArraySwap::swap(index_a, index_b);
    });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

  lockbased::ArraySwap::datum_free(lockbased::ArraySwap::S);
}

void benchmark_mcas_arrayswap(const Configuration& config) {
  lockfree_mcas::ArraySwap::initialize();

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    benchmark(config.n_threads, config.n_ops, u8"swap", [](int random) {
      /* set up random number generator */
      std::random_device rd;
      std::mt19937 engine(rd());
      std::uniform_int_distribution<int> uniform_dist(0, lockfree_mcas::ArraySwap::NUM_ROWS-1);

      int index_a = uniform_dist(engine);
      int index_b = uniform_dist(engine);
      lockfree_mcas::ArraySwap::swap(index_a, index_b);
    });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

  lockfree_mcas::ArraySwap::datum_free(lockfree_mcas::ArraySwap::S);
}

template <typename Deque>
void benchmark_deque(Deque& deque, const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    // prefill deque with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      deque.push_back(uniform_dist(engine));
    }

    benchmark(config.n_threads, config.n_ops, u8"update", [&deque](int random) {
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
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

}

template <typename Deque>
void benchmark_deque_lf(Deque& deque, const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);
  const std::thread::id MAIN_THREAD_ID = std::this_thread::get_id();

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    auto deque_worker = std::move(deque.first);
    auto deque_stealer = std::move(deque.second);
    // prefill deque with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      // deque.push_back(uniform_dist(engine));
      deque_worker.push(uniform_dist(engine));
    }

    benchmark(config.n_threads, config.n_ops, u8"update",
              [&deque, &deque_worker, &deque_stealer, MAIN_THREAD_ID](int random) {
      if (std::this_thread::get_id() == MAIN_THREAD_ID)
      {
        auto choice1 =
            (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
        if (choice1 == 0) {
          deque_worker.push(random % DATA_VALUE_RANGE_MAX);
        } else {
          deque_worker.pop();
        }
      } else {
        auto clone = deque_stealer;
        clone.steal();
      }
    });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

}


template <typename Stack>
void benchmark_stack(Stack& stack, const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    // prefill stack with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      stack.push(uniform_dist(engine));
    }

    benchmark(config.n_threads, config.n_ops, u8"update", [&stack](int random) {
      auto choice =
          (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      if (choice == 0) {
        stack.push(random % DATA_VALUE_RANGE_MAX);
      } else {
        stack.pop();
      }
    });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

}

template <typename Queue>
void benchmark_queue(Queue& queue, const Configuration& config) {
  /* set up random number generator */
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN,
                                                  DATA_VALUE_RANGE_MAX);

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    // prefill queue with 1024 elements
    for (int i = 0; i < DATA_PREFILL; i++) {
      queue.push(uniform_dist(engine));
    }

    benchmark(config.n_threads, config.n_ops, u8"update", [&queue](int random) {
      auto choice =
          (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      if (choice == 0) {
        queue.push(random % DATA_VALUE_RANGE_MAX);
      } else {
        queue.pop();
      }
    });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

}

template <typename List>
void read(List& l, int random) {
  /* read operations: 100% read */
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
  /* mixed operations: 20% update, 80% read */
  auto choice = (random % (10 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
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

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {

    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      list1.insert(uniform_dist(engine));
    }
    benchmark(config.n_threads, config.n_ops, u8"read",
              [&list1](int random) { read(list1, random); });
    benchmark(config.n_threads, config.n_ops, u8"update",
              [&list1](int random) { update(list1, random); });
  }

  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      list2.insert(uniform_dist(engine));
    }
    benchmark(config.n_threads, config.n_ops, u8"mixed", [&list2](int random) { mixed(list2, random); });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

}

template <typename HashMap>
void hm_lookup(HashMap& map, int random) {
  /* read operations: 100% read */
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
  /* mixed operations: 20% update, 80% read */
  auto choice = (random % (10 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
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

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      map1.insert_or_assign(uniform_dist(engine), uniform_dist(engine));
    }
    benchmark(config.n_threads, config.n_ops, u8"read",
              [&map1](int random) { hm_lookup(map1, random); });
    benchmark(config.n_threads, config.n_ops, u8"update",
              [&map1](int random) { hm_update(map1, random); });
  }

  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      map2.insert_or_assign(uniform_dist(engine), uniform_dist(engine));
    }
    benchmark(config.n_threads, config.n_ops, u8"mixed",
              [&map2](int random) { hm_mixed(map2, random); });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

}

template <typename BST>
void bst_lookup(BST& bst) {
  /* read operations: 100% read */
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
  /* mixed operations: 20% update, 80% read */
  auto choice = (random % (10 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
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

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif
  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      bst1.insert(uniform_dist(engine));
    }
    benchmark(config.n_threads, config.n_ops, u8"read",
              [&bst1](int random) { bst_lookup(bst1); });
    benchmark(config.n_threads, config.n_ops, u8"update",
              [&bst1](int random) { bst_update(bst1, random); });
  }

  {
    /* prefill list with 1024 elements */
    for (int i = 0; i < DATA_PREFILL; i++) {
      bst2.insert(uniform_dist(engine));
    }
    benchmark(config.n_threads, config.n_ops, u8"mixed",
              [&bst2](int random) { bst_mixed(bst2, random); });
  }
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

}

void run_benchmarks(const Configuration& config) {
  switch (config.sync_type) {
    case Configuration::SyncType::LOCK: {
      switch (config.benchmarking_algorithm) {
        case Configuration::BenchmarkAlgorithm::MWOBJECT: {
          std::cout << "Benchmark Locking MWObject" << std::endl;
          benchmark_mwobject(config);
        } break;
        case Configuration::BenchmarkAlgorithm::ARRAYSWAP: {
          std::cout << "Benchmark Locking Array Swap" << std::endl;
          benchmark_arrayswap(config);
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
          std::cout << "Benchmark Locking BST" << std::endl;
          lockbased::BinarySearchTree bst1;
          lockbased::BinarySearchTree bst2;
          benchmark_bst(bst1, bst2, config);
        } break;
        case Configuration::ALG_UNDEF: {
          std::cerr << "ALG_UNDEF" << std::endl;
        } break;
      }
    } break;
    case Configuration::SyncType::LOCKFREE: {
      switch (config.benchmarking_algorithm) {
        case Configuration::BenchmarkAlgorithm::MWOBJECT: {
          std::cerr << "MWOBJECT not implemented for lock-free" << std::endl;
        } break;
        case Configuration::BenchmarkAlgorithm::ARRAYSWAP: {
          std::cerr << "ARRAYSWAP not implemented for lock-free" << std::endl;
        } break;
        case Configuration::BenchmarkAlgorithm::STACK: {
          lockfree::Stack<int> stack;
          benchmark_stack(stack, config);
        } break;
        case Configuration::BenchmarkAlgorithm::QUEUE: {
          lockfree::Queue queue;
          benchmark_queue(queue, config);
        } break;
        case Configuration::BenchmarkAlgorithm::DEQUE: {
          auto lf_spmc_deque = lockfree::deque::deque<int>();
          benchmark_deque_lf(lf_spmc_deque, config);
        } break;
        case Configuration::BenchmarkAlgorithm::SORTEDLIST: {
          lockfree::SortedList list1;
          lockfree::SortedList list2;
          benchmark_sorted_list(list1, list2, config);
        } break;
        case Configuration::BenchmarkAlgorithm::HASHMAP: {
          std::cout << "Benchmark Lock-Free HashMap" << std::endl;
          lockfree::HashMap map1;
          lockfree::HashMap map2;
          benchmark_hashmap(map1, map2, config);
        } break;
        case Configuration::BenchmarkAlgorithm::BST: {
          std::cout << "Benchmark Lock-Free BST" << std::endl;
          lockfree::BinarySearchTree bst1;
          // lockfree::BinarySearchTree bst2;
          benchmark_bst(bst1, bst1, config);
        } break;
        case Configuration::ALG_UNDEF: {
          std::cerr << "ALG_UNDEF" << std::endl;
        } break;
      }
    } break;
    case Configuration::SyncType::LOCKFREE_MCAS: {
      switch (config.benchmarking_algorithm) {
        case Configuration::BenchmarkAlgorithm::MWOBJECT: {
          std::cout << "Benchmark Lock-Free MCAS MWObject" << std::endl;
          benchmark_mcas_mwobject(config);
        } break;
        case Configuration::BenchmarkAlgorithm::ARRAYSWAP: {
          std::cout << "Benchmark Lock-Free MCAS Array Swap" << std::endl;
          benchmark_mcas_arrayswap(config);
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
          lockfree_mcas::Deque deque;
          benchmark_deque(deque, config);
        } break;
        case Configuration::BenchmarkAlgorithm::SORTEDLIST: {
          std::cout << "Benchmark Lock-Free MCAS Sorted List" << std::endl;
          lockfree_mcas::SortedList list1;
          lockfree_mcas::SortedList list2;
          benchmark_sorted_list(list1, list2, config);
        } break;
        case Configuration::BenchmarkAlgorithm::HASHMAP: {
          std::cout << "Benchmark Lock-Free MCAS HashMap" << std::endl;
          lockfree_mcas::HashMap map1;
          lockfree_mcas::HashMap map2;
          benchmark_hashmap(map1, map2, config);
        } break;
        case Configuration::BenchmarkAlgorithm::BST: {
          std::cout << "Benchmark Lock-Free MCAS BST" << std::endl;
          lockfree_mcas::BinarySearchTree bst1;
          lockfree_mcas::BinarySearchTree bst2;
          benchmark_bst(bst1, bst2, config);
        } break;
        case Configuration::ALG_UNDEF: {
          std::cerr << "ALG_UNDEF" << std::endl;
        } break;
      }
    } break;
    case Configuration::SYNC_UNDEF: {
      std::cerr << "SYNC_UNDEF" << std::endl;
    } break;
  }

}

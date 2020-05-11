#include "benchmark.h"
#include "Deque.h"

static const int DATA_VALUE_RANGE_MIN = 0;
static const int DATA_VALUE_RANGE_MAX = 256;
static const int DATA_PREFILL = 512;

template<typename Deque>
void benchmark_deque(Deque &deque, int n_threads) {
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

    benchmark(n_threads, u8"update", [&deque](int random) {
      deque.push_front(random % DATA_VALUE_RANGE_MAX);
      // auto choice1 =
      //     (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      // auto choice2 =
      //     (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
      // if (choice1 == 0) {
      //   if (choice2 == 0) {
      //     deque.push_back(random % DATA_VALUE_RANGE_MAX);
      //   } else {
      //     // deque.push_front(random % DATA_VALUE_RANGE_MAX);
      //   }
      // } else {
      //   if (choice2 == 0) {
      //     // deque.pop_back();
      //   } else {
      //     deque.pop_front();
      //   }
      // }
    });
  }
}

int main() {
    lockfree_mcas::Deque deque;
    benchmark_deque(deque, 2);
}

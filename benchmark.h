/* a small benchmarking framework by David Klaftenegger, 2015
 * please report bugs or suggest improvements to david.klaftenegger@it.uu.se
 */


#pragma once

#include <atomic>
#include <chrono>
#include <random>
#include <thread>
#include <vector>
#include <iostream>

enum class worker_status {wait, work, finish};

static const int RANDOM_VALUE_RANGE_MIN = 0;
static const int RANDOM_VALUE_RANGE_MAX = 65536;

/* template is used to allow functions/functors of any signature */
template<typename Function>
void worker(unsigned int random_seed, unsigned int n_ops, Function fun) {
  /* set up random number generator */
  std::mt19937 engine(random_seed);
  std::uniform_int_distribution<int> uniform_dist(RANDOM_VALUE_RANGE_MIN, RANDOM_VALUE_RANGE_MAX);

  for (unsigned int i = 0; i < n_ops; i++) {
    auto random = uniform_dist(engine);
    /* do specified work */
    fun(random);
  }
}


template<typename Function>
void benchmark(unsigned int threadcnt, unsigned int n_ops,
               const std::string& identifier, Function fun) {
  auto n_ops_per_thread = n_ops / threadcnt;
  /* spawn workers */
  std::vector<std::thread*> workers;
  std::random_device rd;

  using clock = std::chrono::high_resolution_clock;
  std::chrono::time_point<clock> start_time = clock::now();

  for(unsigned int i = 0; i < threadcnt-1; i++) {
    auto seed = rd();
    auto w = new std::thread([seed, n_ops_per_thread, fun]() { worker(seed, n_ops_per_thread, fun); });
    workers.push_back(w);
    // set thread affinity for workers
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(i+1, &cpuset);
    pthread_setaffinity_np(workers[i]->native_handle(), sizeof(cpu_set_t), &cpuset);
  };

  // set thread affinity for main thread
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

  worker(rd(), n_ops_per_thread, fun);

  /* make sure all workers terminated */
  for(auto& w : workers) {
    w->join();
    delete w;
  }
  std::chrono::time_point<clock> end_time = clock::now();
  workers.clear();

  long time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
  std::cout << identifier << std::endl << u8"\tthreads: " << threadcnt
            << u8" - ops: " << n_ops
            << u8" - time: " << time << "ms" << "\n";
}

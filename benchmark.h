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
void worker(unsigned int random_seed, double& time,
            unsigned int n_ops, Function fun) {
  /* set up random number generator */
  std::mt19937 engine(random_seed);
  std::uniform_int_distribution<int> uniform_dist(RANDOM_VALUE_RANGE_MIN, RANDOM_VALUE_RANGE_MAX);
  /* for time measurements */
  typedef std::chrono::high_resolution_clock clock;
  std::chrono::time_point<clock> start_time = clock::now();

  for (unsigned int i = 0; i < n_ops; i++) {
    auto random = uniform_dist(engine);
    /* do specified work */
    fun(random);
  }

  std::chrono::time_point<clock> end_time = clock::now();
  time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
}


template<typename Function>
void benchmark(unsigned int threadcnt, unsigned int n_ops,
               const std::string& identifier, Function fun) {
  /* spawn workers */
  std::vector<double> times(threadcnt);
  std::vector<std::thread*> workers;
  std::random_device rd;
  for(unsigned int i = 0; i < threadcnt-1; i++) {
    auto seed = rd();
    auto& t = times[i];
    auto w = new std::thread([seed, &t, n_ops, fun]() { worker(seed, t, n_ops, fun); });
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

  auto& main_thread_time = times[0];
  worker(rd(), main_thread_time, n_ops, fun);

  /* make sure all workers terminated */
  for(auto& w : workers) {
    w->join();
    delete w;
  }
  workers.clear();

  /* compute sum of partial results */
  double result = 0.0;
  for(auto& v : times) {
    result += v;
  }

  result = result / threadcnt;
  std::cout << identifier << std::endl << u8"\tthreads: " << threadcnt
            << u8" - ops: " << n_ops
            << u8" - time: "<< std::fixed << result << "\n";
}

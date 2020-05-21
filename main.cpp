#include <iostream>

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#include "benchmarks.h"
#include "configuration.h"
#include "cxxopts.hpp"

int main(int argc, char *argv[])
{
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_bench_begin (__splash2_barnes);
#endif

  // parse options
  cxxopts::Options options("mcas_benchmark", "A brief description");

  options.add_options()
      ("n,nthreads", "Number of threads", cxxopts::value<int>()->default_value("1"))
      ("i,iter", "Number of iterations", cxxopts::value<int>()->default_value("1"))
      ("o,ops", "Number of operations per thread", cxxopts::value<int>()->default_value("100"))
      ("s,sync", "Synchronization type: lock, lockfree, lockfree-mcas", cxxopts::value<std::string>())
      ("a,algorithm", "Benchmark algorithm: mwobject, stack, queue, deque, sorted-list, hashmap, bst", cxxopts::value<std::string>())
      ("d,debug", "Enable debugging", cxxopts::value<bool>()->default_value("false"))
      ("h,help", "Print usage")
      ;

  auto result = options.parse(argc, argv);

  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  // generate configuration
  Configuration conf;

  conf.debug = result["debug"].as<bool>();
  conf.n_threads = result["nthreads"].as<int>();
  conf.n_iter = result["iter"].as<int>();
  conf.n_ops = result["ops"].as<int>();
  conf.sync_type = Configuration::SyncType::SYNC_UNDEF;
  conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::ALG_UNDEF;

  if (result.count("sync")) {
    std::string sync_type = result["sync"].as<std::string>();
    if (sync_type == "lock") conf.sync_type = Configuration::SyncType::LOCK;
    if (sync_type == "lockfree") conf.sync_type = Configuration::SyncType::LOCKFREE;
    if (sync_type == "lockfree-mcas") conf.sync_type = Configuration::SyncType::LOCKFREE_MCAS;
  }

  if (conf.sync_type == Configuration::SyncType::SYNC_UNDEF) {
    std::cout << "sync type is not defined" << std::endl;
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (result.count("algorithm")) {
    std::string algorithm = result["algorithm"].as<std::string>();
    if (algorithm == "mwobject") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::MWOBJECT;
    if (algorithm == "stack") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::STACK;
    if (algorithm == "queue") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::QUEUE;
    if (algorithm == "deque") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::DEQUE;
    if (algorithm == "sorted-list") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::SORTEDLIST;
    if (algorithm == "hashmap") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::HASHMAP;
    if (algorithm == "bst") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::BST;
  }

  if (conf.benchmarking_algorithm == Configuration::BenchmarkAlgorithm::ALG_UNDEF) {
    std::cout << "algorithm is not defined" << std::endl;
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (conf.debug) {
    std::cout << "configuration:" << std::endl
              << "debug = " << conf.debug << std::endl
              << "n_iter = " << conf.n_iter << std::endl
              << "n_threads = " << conf.n_threads << std::endl
              << "n_ops = " << conf.n_ops << std::endl
              << "type = " << conf.sync_type << std::endl
              << "algorithm = " << conf.benchmarking_algorithm << std::endl;
  }

  std::cout << "MCAS Benchmarks started" << std::endl;
  run_benchmarks(conf);
  std::cout << "MCAS Benchmarks finished" << std::endl;

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_bench_end();
#endif
  return 0;
}

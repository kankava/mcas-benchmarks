#include <iostream>
#include "benchmarks.h"
#include "configuration.h"
#include "cxxopts.hpp"

int main(int argc, char *argv[])
{
  // parse options
  cxxopts::Options options("mcas_benchmark", "A brief description");

  options.add_options()
      ("n,nthreads", "Number of threads", cxxopts::value<int>()->default_value("1"))
      ("i,iter", "Number of iterations", cxxopts::value<int>()->default_value("1"))
      ("t,type", "Synchronization type: lock, lockfree, lockfree-mcas", cxxopts::value<std::string>())
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

  if (result.count("type")) {
    std::string sync_type = result["type"].as<std::string>();
    if (sync_type == "lock") conf.sync_type = Configuration::SyncType::LOCK;
    if (sync_type == "lockfree") conf.sync_type = Configuration::SyncType::LOCKFREE;
    if (sync_type == "lockfree-mcas") conf.sync_type = Configuration::SyncType::LOCKFREE_MCAS;
  }

  if (result.count("algorithm")) {
    std::string sync_type = result["algorithm"].as<std::string>();
    if (sync_type == "mwobject") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::MWOBJECT;
    if (sync_type == "stack") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::STACK;
    if (sync_type == "queue") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::QUEUE;
    if (sync_type == "deque") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::DEQUE;
    if (sync_type == "sorted-list") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::SORTEDLIST;
    if (sync_type == "hashmap") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::HASHMAP;
    if (sync_type == "bst") conf.benchmarking_algorithm = Configuration::BenchmarkAlgorithm::BST;
  }

  // std::cout << "configuration:" << std::endl
  //           << "debug = " << conf.debug << std::endl
  //           << "n iter = " << conf.n_iter << std::endl
  //           << "n threads = " << conf.n_threads << std::endl
  //           << "type = " << conf.sync_type << std::endl
  //           << "algorithm = " << conf.benchmarking_algorithm << std::endl;

  std::cout << "MCAS Benchmarks started" << std::endl;

  run_benchmarks(conf);

  std::cout << "MCAS Benchmarks finished" << std::endl;

  return 0;
}

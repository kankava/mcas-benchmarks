#pragma once

class Configuration{
public:
  enum SyncType{
    SYNC_UNDEF,
    LOCK,
    LOCKFREE,
    LOCKFREE_MCAS
  };

  enum BenchmarkAlgorithm{
    ALG_UNDEF,
    MWOBJECT,
    STACK,
    QUEUE,
    DEQUE,
    SORTEDLIST,
    HASHMAP,
    BST,
  };

  Configuration(){
    n_threads = 1;
    sync_type = SYNC_UNDEF;
    benchmarking_algorithm = ALG_UNDEF;
    n_iter = 1;
    n_ops = 100;
    debug = false;
  };

  SyncType sync_type;
  BenchmarkAlgorithm benchmarking_algorithm;
  unsigned int n_threads;
  unsigned int n_iter;
  unsigned int n_ops;
  bool debug;
  static const Configuration default_conf;
};

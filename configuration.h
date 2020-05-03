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
    time = 1;
    debug = false;
  };

  SyncType sync_type;
  BenchmarkAlgorithm benchmarking_algorithm;
  int n_threads;
  int n_iter;
  int time;
  bool debug;
  static const Configuration default_conf;
};

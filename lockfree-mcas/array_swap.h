#pragma once

#include <sys/time.h>
#include <cassert>
#include <cstdint>
#include <memory>
#include "../mcas/mcas.h"

namespace lockfree_mcas {
namespace ArraySwap {

#define NUM_SUB_ITEMS 2
// #define NUM_OPS 10000000
#define NUM_ROWS 100000
// #define NUM_THREADS 1

struct Element {
  int32_t value_[NUM_SUB_ITEMS];
};

struct Datum {
  // pointer to the hashmap
  Element* elements_;
};

struct sps {
  Datum* array;
  int num_rows_;
  int num_sub_items_;
};

sps* S;

void datum_init(sps* s) {
  for (int i = 0; i < NUM_ROWS; i++) {
    s->array[i].elements_ = (Element*)malloc(sizeof(Element));

    for (int j = 0; j < NUM_SUB_ITEMS; j++)
      s->array[i].elements_->value_[j] = i + j;
  }
}

void datum_free(sps* s) {
  for (int i = 0; i < NUM_ROWS; i++) {
    free(s->array[i].elements_);
  }
}

void initialize() {
  S = (sps*)malloc(sizeof(sps));
  S->num_rows_ = NUM_ROWS;
  S->num_sub_items_ = NUM_SUB_ITEMS;
  S->array = (Datum*)malloc(sizeof(Datum) * NUM_ROWS);
  datum_init(S);

  fprintf(stderr, "Created array at %p\n", (void*)S);
}

bool swap(unsigned int index_a, unsigned int index_b) {
  // check if index is out of array
  assert(index_a < NUM_ROWS && index_b < NUM_ROWS);

  // exit if swapping the same index
  if (index_a == index_b) return true;

  // enforce index_a < index_b
  if (index_a > index_b) {
    unsigned int index_tmp = index_a;
    index_a = index_b;
    index_b = index_tmp;
  }

  Element* addr_a = S->array[index_a].elements_;
  Element* addr_b = S->array[index_b].elements_;

  dcas(reinterpret_cast<uint64_t*>(&S->array[index_a].elements_),
       reinterpret_cast<uint64_t>(addr_a), reinterpret_cast<uint64_t>(addr_b),
       reinterpret_cast<uint64_t*>(&S->array[index_b].elements_),
       reinterpret_cast<uint64_t>(addr_b), reinterpret_cast<uint64_t>(addr_a));

  return true;
}

struct args {
  unsigned int num_threads;
  unsigned int num_ops;
};

void* run_stub(void* ptr) {
  unsigned int num_ops = ((struct args*)ptr)->num_ops;
  unsigned int num_threads = ((struct args*)ptr)->num_threads;
  for (int i = 0; i < num_ops / num_threads; ++i) {
    int index_a = rand() % NUM_ROWS;
    int index_b = rand() % NUM_ROWS;
    swap(index_a, index_b);
  }
  return nullptr;
}

int run_benchmark(unsigned int num_threads, unsigned int num_ops) {
  struct timeval tv_start;
  struct timeval tv_end;

  // This contains the Atlas restart code to find any reusable data
  initialize();
  struct args* Args = (struct args*)malloc(sizeof(struct args));
  Args->num_threads = num_threads;
  Args->num_ops = num_ops;

  pthread_t threads[num_threads];

  gettimeofday(&tv_start, NULL);
  for (int i = 0; i < num_threads; ++i) {
    pthread_create(&threads[i], NULL, &run_stub, (void*)Args);
  }

  for (int i = 0; i < num_threads; ++i) {
    pthread_join(threads[i], NULL);
  }

  gettimeofday(&tv_end, NULL);

  datum_free(S);
  free(S->array);
  free(S);
  return (tv_end.tv_usec - tv_start.tv_usec) +
         (tv_end.tv_sec - tv_start.tv_sec) * 1000000;
}

}  // namespace arrayswap
}  // namespace lockfree_mcas

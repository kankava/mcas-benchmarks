#ifndef __CASN_H__
#define __CASN_H__

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#include <cstdlib>
#else
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"
#endif

#define MAX_CASN_ENTRIES 4
#define UNDECIDED        0
#define FAILED           1
#define SUCCEEDED        2

typedef struct RDCSSDescriptor {
  uint64_t *a1;
  uint64_t o1;
  uint64_t *a2;
  uint64_t o2;
  uint64_t n2;
} RDCSSDescriptor;

typedef struct CASNEntry {
  uint64_t *Address;
  uint64_t Old;
  uint64_t New;
} CASNEntry;

typedef struct CASNDescriptor {
  size_t n_entries;
  uint64_t status;
  CASNEntry entries[MAX_CASN_ENTRIES];  // TODO: use flexible array member???
} CASNDescriptor;

uint64_t get_rdcss_descriptor_ptr(RDCSSDescriptor *d);

RDCSSDescriptor *get_rdcss_descriptor(uint64_t ptr);

uint64_t rdcss(RDCSSDescriptor *d);

uint64_t rdcss_read(uint64_t *addr);

bool casn(CASNDescriptor *cd);

uint64_t casn_read(uint64_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* __CASN_H__ */

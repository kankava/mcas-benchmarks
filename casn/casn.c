#include "casn.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

uint64_t cas1(uint64_t *addr0, uint64_t old0, uint64_t new0) {
  atomic_compare_exchange_weak(addr0, &old0, new0);
  return old0;
}

void complete(RDCSSDescriptor *d);

bool is_rdcss_descriptor(uint64_t ptr) { return (ptr >> 63u) == 1; }

uint64_t get_rdcss_descriptor_ptr(RDCSSDescriptor *d) {
  return (uint64_t)d | (uint64_t)1 << 63u;
}

RDCSSDescriptor *get_rdcss_descriptor(uint64_t ptr) {
  return (RDCSSDescriptor *)(ptr & ~((uint64_t)1 << 63u));
}

uint64_t rdcss(RDCSSDescriptor *d) {
  uint64_t r;

  while (true) {
    r = cas1((uint64_t *)d->a2, d->o2, get_rdcss_descriptor_ptr(d));
    if (is_rdcss_descriptor(r)) {
      complete(get_rdcss_descriptor(r));
    } else {
      break;
    }
  }

  if (r == d->o2) {
    complete(d);
  }

  return r;
}

uint64_t rdcss_read(uint64_t *addr) {
  uint64_t r;
  while (true) {
    r = atomic_load_explicit(addr, memory_order_seq_cst);
    if (is_rdcss_descriptor(r)) {
      complete(get_rdcss_descriptor(r));
    } else {
      break;
    }
  }

  return r;
}

void complete(RDCSSDescriptor *d) {
  uint64_t a1 = (uint64_t)d->a1;
  if (a1 == d->o1) {
    cas1((uint64_t *)d->a2, get_rdcss_descriptor_ptr(d), d->n2);
  } else {
    cas1((uint64_t *)d->a2, get_rdcss_descriptor_ptr(d), d->o2);
  }
}

bool is_casn_descriptor(uint64_t ptr) { return (ptr >> 62u == 1); }

uint64_t get_casn_descriptor_ptr(CASNDescriptor *d) {
  return (uint64_t)d | (uint64_t)1 << 62u;
}

CASNDescriptor *get_casn_descriptor(uint64_t ptr) {
  return (CASNDescriptor *)(ptr & ~((uint64_t)1 << 62u));
}

bool casn(CASNDescriptor *cd) {
  // if (atomic_load_explicit(&(cd->status), memory_order_seq_cst) == UNDECIDED) {
    if (cd->status == UNDECIDED) {
    uint64_t status = SUCCEEDED;

    for (int i = 0; i < cd->n_entries && status == SUCCEEDED; i++) {
    retry:;  // empty statement
      RDCSSDescriptor desc;
      desc.a1 = &cd->status;
      desc.o1 = UNDECIDED;
      desc.a2 = cd->entries[i].Address;
      desc.o2 = cd->entries[i].Old;
      desc.n2 = get_casn_descriptor_ptr(cd);

      uint64_t val = rdcss(&desc);
      if (is_casn_descriptor(val)) {
        if (val != get_casn_descriptor_ptr(cd)) {
          casn(get_casn_descriptor(val));
          goto retry;
        }
      } else if (val != cd->entries[i].Old) {
        status = FAILED;
      }
    }
    cas1(&cd->status, UNDECIDED, status);
  }

  bool success = cd->status == SUCCEEDED;
  int n = cd->n_entries;
  for (int i = 0; i < n; i++) {
    uint64_t new1;
    if (success) {
      new1 = cd->entries[i].New;
    } else {
      new1 = cd->entries[i].Old;
    }
    uint64_t *addr0 = cd->entries[i].Address;
    cas1(addr0, get_casn_descriptor_ptr(cd), new1);
  }
  return success;
}

uint64_t casn_read(uint64_t *addr) {
  uint64_t r;
  while (true) {
    r = rdcss_read(addr);
    // r = atomic_load_explicit(addr, memory_order_seq_cst);
    if (is_casn_descriptor(r)) {
      casn((get_casn_descriptor(r)));
    } else {
      break;
    }
  }
  return r;
}

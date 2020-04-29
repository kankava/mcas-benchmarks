// software implementation of multi-word cas in c
// described in paper by harris et al.

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>

uintptr_t CAS1(uintptr_t *addr1, uintptr_t old1, uintptr_t new1) {
  uintptr_t old = old1;
  atomic_compare_exchange_strong(a, &old, n);
  return old;
}

void complete(RDCSSDescriptor *d);

int is_descriptor(uintptr_t ptr);

typedef struct RDCSSDescriptor {
  uintptr_t *a1;
  uintptr_t o1;
  uintptr_t *a2;
  uintptr_t o2;
  uintptr_t n2;
} RdcssDescriptor;

uintptr_t RDCSS(RDCSSDescriptor *d) {
    uintptr_t r;
    uintptr_t d_ptr = (((uintptr_t)(void*)d) | 1);
    do {
        r = CAS1(d->a2, d->o2, d_ptr);
        if (is_descriptor(r)) {
            complete((RdcssDescriptor*)(r&~1));
        }
    } while (is_descriptor(r));
    
    if (r == d->o2) complete(d);

    return r;
}

void complete(RdcssDescriptor *d) {
    uintptr_t v = atomic_load(d->a1);
    uintptr_t d_ptr = (((uintptr_t)(void*)d) | 1);
    if (v == d->o1) {
        CAS1(d->a2,d_ptr,d->n2);
    }
    else {
        CAS1(d->a2,d_ptr,d->o2);
    }
}

int is_descriptor(uintptr_t ptr) {
    return ptr & 1;
}

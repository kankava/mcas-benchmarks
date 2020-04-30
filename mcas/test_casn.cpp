#include <atomic>
#include <iostream>
#include <memory>
#include <vector>

namespace casn {
// static const uint64_t RDCSSDescriptorFlag = (uint64_t)1 << 63;
// static const uint64_t CondCASFlag = (uint64_t)1 << 62;

static const uint64_t UNDECIDED = 0;
static const uint64_t FAILED = 0;
static const uint64_t SUCCEEDED = 0;


struct RDCSSDescriptor {
  std::atomic<void *> a1;
  void *o1;
  std::atomic<void *> a2;
  void *o2;
  void *n2;
};

void* rdcss(RDCSSDescriptor *d);

void complete(RDCSSDescriptor *d);

bool is_rdcss_descriptor(void *ptr) {
  return ((uint64_t)ptr >> 63 == 1);
}

uint64_t get_rdcss_descriptor_ptr(RDCSSDescriptor *d) {
    return (uint64_t) d | (uint64_t)1 << 63;
}

RDCSSDescriptor *get_rdcss_descriptor(uint64_t ptr) {
    return (RDCSSDescriptor *) (ptr & ~((uint64_t)1 << 63));
}

void* rdcss(RDCSSDescriptor *d) {
  void* oldValue;
  void* descriptor_void_ptr = (void *)get_rdcss_descriptor_ptr(d);
  do {
    if (!d->a2.compare_exchange_weak(d->o2, descriptor_void_ptr)) {
      oldValue = d->a2.load();
      if (is_rdcss_descriptor(oldValue)) {
	    complete(get_rdcss_descriptor((uint64_t) oldValue));
      }
    } else {
      oldValue = d->o2;
    }
  } while (is_rdcss_descriptor(oldValue));

  if (oldValue == d->o2) {
    complete(d);
  }

  return oldValue;
}

void complete(RDCSSDescriptor *d) {
  void* controlValue = d->a1.load();
  void* descriptor_void_ptr = (void *)get_rdcss_descriptor_ptr(d);

  if (controlValue == d->o1) {
    d->a2.compare_exchange_weak(descriptor_void_ptr, d->n2);
  } else {
    d->a2.compare_exchange_weak(descriptor_void_ptr, d->o2);
  }
}

struct Update {
    std::atomic<void *> addr1;
    void* old1;
    void* new1;
};

struct CASNDescriptor {
    uint64_t status;
    std::vector<Update> updates;
};

bool is_casn_descriptor(void *ptr) {
    return ((uint64_t) ptr >> 62 == 1);
}

uint64_t get_casn_descriptor_ptr(CASNDescriptor *cd) {
    return (uint64_t) cd | (uint64_t)1 << 62;
}

CASNDescriptor *get_casn_descriptor(uint64_t ptr) {
    return (CASNDescriptor *) (ptr & ~((uint64_t)1 << 62));
}

/*
bool casn(CASNDescriptor *cd) {
    if (cd->status) {
        uint64_t status = SUCCEEDED;
        std::vector<RDCSSDescriptor> descs;

        for (int i = 0; i < descs.size() && status == SUCCEEDED; i++) {
        retry:
            RDCSSDescriptor new_desc;
            new_desc.a1.store(&cd.status);
            new_desc.o1 = &cd.status,
            new_desc.a2.store(&cd.updates[i].addr1.load());
            new_desc.o2 = ;
            new_desc.n2 = get_casn_descriptor_ptr
            descs.pushh_back(new_desc);
        }
    }
}
*/

}

int main() {
/*  int a = 1, b = 2;
  int* aptr = &a, *bptr = &b;
  int c = 5;
  int* cptr = &c;

  casn::RDCSSDescriptor r;
  r.a1.store(aptr);
  r.o1 = aptr;
  r.a2.store(bptr);
  r.o2 = bptr;
  r.n2 = cptr;

  std::cout << * (int *)(r.a2.load()) << std::endl;
  casn::rdcss(&r);
  std::cout << * (int *)(r.a2.load()) << std::endl;
*/

  std::shared_ptr<int> aptr = std::make_shared<int>(1);
  std::shared_ptr<int> bptr = std::make_shared<int>(2);
  std::shared_ptr<int> cptr = std::make_shared<int>(5);

  // int a = 1, b = 2;
  // int* aptr = &a, *bptr = &b;
  // int c = 5;
  // int* cptr = &c;

  casn::RDCSSDescriptor r;
  r.a1.store(&aptr);
  r.o1 = &aptr;
  r.a2.store(&bptr);
  r.o2 = &bptr;
  r.n2 = &cptr;

  std::cout << * *(std::shared_ptr<int>  *)(r.a2.load()) << std::endl;
  casn::rdcss(&r);
  std::cout << * *(std::shared_ptr<int>  *)(r.a2.load()) << std::endl;
}

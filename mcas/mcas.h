//
// MCAS software implementation as described in Harris et al.
// paper - A Practical Multi-Word Compare-and-Swap Operation
//

#pragma once

#include <atomic>
#include <memory>

// template <typename T>
// struct RDCSS_Descriptor {
//   T* addr1;
//   T old1;
//   T* addr2;
//   T old2;
//   T new2;
//   bool committed;
// };
//
// template <typename T>
// bool RDCSS(RDCSS_Descriptor &descriptor) {
//   return false;
// }
//
// template <typename T>
// T RDCSSRead(T *addr) {
//   T r;
//   do {
//     r = *addr;
//     if (is_descriptor(r)) complete(r);
//   } while (is_descriptor(r));
//   return r;
// }
//
//
// template <typename T>
// void complete(RDCSS_Descriptor<T> &descriptor) {
//   auto v = descriptor.addr1;
//   if (v == descriptor.old1) {
//     std::atomic_compare_exchange_weak(descriptor.addr2, descriptor, descriptor.new2);
//   } else {
//     std::atomic_compare_exchange_weak(descriptor.addr2, descriptor, descriptor.old2);
//   }
//
// }


template <typename T>
inline bool CAS(T *addr1, T old1, T new1) {
  // atomically
  // std::lock_guard<std::mutex> lock(global_cas_lock);
  {
    if (*addr1 == old1) {
      *addr1 = new1;
      return true;
    }
    return false;
  }
}

template <typename T>
inline bool CAS(T *addr1, T *addr2, T old1, T old2, T new1, T new2) {
  // atomically
  // std::lock_guard<std::mutex> lock(global_cas_lock);
  {
    if ((*addr1 == old1) && (*addr2 == old2)) {
      *addr1 = new1;
      *addr2 = new2;
      return true;
    }
    return false;
  }
}

template <typename T>
inline bool CAS(T *addr1, T *addr2, T *addr3, T old1, T old2, T old3, T new1, T new2,
         T new3) {
  // atomically
  // std::lock_guard<std::mutex> lock(global_cas_lock);
  {
    if ((*addr1 == old1) && (*addr2 == old2) && (*addr3 == old3)) {
      *addr1 = new1;
      *addr2 = new2;
      *addr3 = new3;
      return true;
    }
    return false;
  }
}

template <typename T>
inline bool CAS(T *addr1, T *addr2, T *addr3, T *addr4, T old1, T old2, T old3, T old4,
         T new1, T new2, T new3, T new4) {
  // atomically
  // std::lock_guard<std::mutex> lock(global_cas_lock);
  {
    if ((*addr1 == old1) && (*addr2 == old2) && (*addr3 == old3) &&
        (*addr4 == old4)) {
      *addr1 = new1;
      *addr2 = new2;
      *addr3 = new3;
      *addr4 = new4;
      return true;
    }
    return false;
  }
}

template <typename T>
inline bool DCAS(T *addr1, T *addr2, T old1, T old2, T new1, T new2) {
  return CAS(addr1, addr2, old1, old2, new1, new2);
}

template <typename T>
inline bool CAS3(T *addr1, T *addr2, T *addr3, T old1, T old2, T old3, T new1, T new2,
          T new3) {
  return CAS(addr1, addr2, addr3, old1, old2, old3, new1, new2, new3);
}

template <typename T>
inline bool CAS4(T *addr1, T *addr2, T *addr3, T *addr4, T old1, T old2, T old3, T old4,
                T new1, T new2, T new3, T new4) {
    return CAS(addr1, addr2, addr3, addr4, old1, old2, old3, old4, new1, new2, new3, new4);
}

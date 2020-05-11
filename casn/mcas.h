#ifndef __GEM_ASM_MCAS_H__
#define __GEM_ASM_MCAS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef MCAS_GEM5

#define MCAS_READ(PTR) *ptr

inline __attribute__ ((always_inline)) uint64_t cas(uint64_t* addr0, uint64_t old0, uint64_t new0) {
  register uint64_t rax asm("rax") = (uint64_t)addr0;
  register uint64_t rdx asm("rdx") = (uint64_t)old0;
  register uint64_t rcx asm("rcx") = (uint64_t)new0;

  asm ("\
        .byte 0xf0, 0x48, 0x69, 0xc0, 0x00, 0x00, 0x00, 0x80\n\
       " : "=a"(rax) : "r"(rax), "r"(rdx), "r"(rcx) :);

  return rax;
}

inline __attribute__ ((always_inline)) uint64_t dcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
                                                     uint64_t* addr1, uint64_t old1, uint64_t new1) {
  register uint64_t rax asm("rax") = (uint64_t)addr0;
  register uint64_t rdx asm("rdx") = (uint64_t)old0;
  register uint64_t rcx asm("rcx") = (uint64_t)new0;
  register uint64_t rsi asm("rsi") = (uint64_t)addr1;
  register uint64_t rdi asm("rdi") = (uint64_t)old1;
  register uint64_t r8 asm("r8") = (uint64_t)new1;

  asm ("\
        .byte 0xf0, 0x48, 0x69, 0xc0, 0x01, 0x00, 0x00, 0x80\n\
       " : "=a"(rax) : "r"(rax), "r"(rdx), "r"(rcx), "r"(rsi), "r"(rdi), "r"(r8) :);

  return rax;
}

inline __attribute__ ((always_inline)) uint64_t tcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
                                                     uint64_t* addr1, uint64_t old1, uint64_t new1,
                                                     uint64_t* addr2, uint64_t old2, uint64_t new2) {
  register uint64_t rax asm("rax") = (uint64_t)addr0;
  register uint64_t rdx asm("rdx") = (uint64_t)old0;
  register uint64_t rcx asm("rcx") = (uint64_t)new0;
  register uint64_t rsi asm("rsi") = (uint64_t)addr1;
  register uint64_t rdi asm("rdi") = (uint64_t)old1;
  register uint64_t r8 asm("r8") = (uint64_t)new1;
  register uint64_t r9 asm("r9") = (uint64_t)addr2;
  register uint64_t r10 asm("r10") = (uint64_t)old2;
  register uint64_t r11 asm("r11") = (uint64_t)new2;

  asm ("\
        .byte 0xf0, 0x48, 0x69, 0xc0, 0x02, 0x00, 0x00, 0x80\n\
       " : "=a"(rax) : "r"(rax), "r"(rdx), "r"(rcx), "r"(rsi), "r"(rdi), "r"(r8), "r"(r9), "r"(r10), "r"(r11) :);

  return rax;
}

inline __attribute__ ((always_inline)) uint64_t qcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
                                                     uint64_t* addr1, uint64_t old1, uint64_t new1,
                                                     uint64_t* addr2, uint64_t old2, uint64_t new2,
                                                     uint64_t* addr3, uint64_t old3, uint64_t new3) {

  register uint64_t rax asm("rax") = (uint64_t)addr0;
  register uint64_t rdx asm("rdx") = (uint64_t)old0;
  register uint64_t rcx asm("rcx") = (uint64_t)new0;
  register uint64_t rsi asm("rsi") = (uint64_t)addr1;
  register uint64_t rdi asm("rdi") = (uint64_t)old1;
  register uint64_t r8 asm("r8") = (uint64_t)new1;
  register uint64_t r9 asm("r9") = (uint64_t)addr2;
  register uint64_t r10 asm("r10") = (uint64_t)old2;
  register uint64_t r11 asm("r11") = (uint64_t)new2;
  register uint64_t r12 asm("r12") = (uint64_t)addr3;
  register uint64_t r13 asm("r13") = (uint64_t)old3;
  register uint64_t r14 asm("r14") = (uint64_t)new3;

  asm ("\
        .byte 0xf0, 0x48, 0x69, 0xc0, 0x03, 0x00, 0x00, 0x80\n\
       " : "=a"(rax) : "r"(rax), "r"(rdx), "r"(rcx), "r"(rsi), "r"(rdi), "r"(r8), "r"(r9), "r"(r10), "r"(r11),  "r"(r12), "r"(r13), "r"(r14) :);

  return rax;
}

#define CAS_READ()
inline __attribute__ ((always_inline)) uint64_t cas_read(uint64_t* addr) {
  return *addr;
}

#else

#include "casn.h"

#define MCAS_READ(PTR) casn_read((uint64_t*) PTR)

// inline __attribute__ ((always_inline))
uint64_t cas(uint64_t* addr0, uint64_t old0, uint64_t new0) {
  CASNDescriptor casn_desc = {1,
                              UNDECIDED,
                              {
                                  {addr0, old0, new0},
                                  {nullptr, 0, 0},
                                  {nullptr, 0, 0},
                                  {nullptr, 0, 0},
                              }};
  return casn(&casn_desc);
}

// inline __attribute__ ((always_inline))
uint64_t dcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
              uint64_t* addr1, uint64_t old1, uint64_t new1) {
  CASNDescriptor casn_desc = {2,
                              UNDECIDED,
                              {
                                  {addr0, old0, new0},
                                  {addr1, old1, new1},
                                  {nullptr, 0, 0},
                                  {nullptr, 0, 0},
                              }};
  return casn(&casn_desc);
}

// inline __attribute__ ((always_inline))
uint64_t tcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
              uint64_t* addr1, uint64_t old1, uint64_t new1,
              uint64_t* addr2, uint64_t old2, uint64_t new2) {
  CASNDescriptor casn_desc = {3,
                              UNDECIDED,
                              {
                                  {addr0, old0, new0},
                                  {addr1, old1, new1},
                                  {addr2, old2, new2},
                                  {nullptr, 0, 0},
                              }};
  return casn(&casn_desc);
}

// inline __attribute__ ((always_inline))
uint64_t qcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
              uint64_t* addr1, uint64_t old1, uint64_t new1,
              uint64_t* addr2, uint64_t old2, uint64_t new2,
              uint64_t* addr3, uint64_t old3, uint64_t new3) {
  CASNDescriptor casn_desc = {4,
                              UNDECIDED,
                              {
                                  {addr0, old0, new0},
                                  {addr1, old1, new1},
                                  {addr2, old2, new2},
                                  {addr3, old3, new3},
                              }};
  return casn(&casn_desc);
}

#endif /* MCASGEM5 */

#ifdef __cplusplus
}
#endif

#endif /* __GEM_ASM_MCAS_H__ */

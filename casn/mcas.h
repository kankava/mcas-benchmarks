#ifndef __GEM_ASM_MCAS_H__
#define __GEM_ASM_MCAS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint64_t cas(uint64_t* addr0, uint64_t old0, uint64_t new0);

uint64_t dcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
              uint64_t* addr1, uint64_t old1, uint64_t new1);

uint64_t tcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
              uint64_t* addr1, uint64_t old1, uint64_t new1,
              uint64_t* addr2, uint64_t old2, uint64_t new2);

uint64_t qcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
              uint64_t* addr1, uint64_t old1, uint64_t new1,
              uint64_t* addr2, uint64_t old2, uint64_t new2,
              uint64_t* addr3, uint64_t old3, uint64_t new3);

#ifdef __cplusplus
}
#endif

#endif /* __GEM_ASM_MCAS_H__ */

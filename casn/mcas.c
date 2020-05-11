// #include "mcas.h"
// #include "casn.h"
//
// uint64_t cas(uint64_t* addr0, uint64_t old0, uint64_t new0) {
//   CASNDescriptor casn_desc = {1,
//                               UNDECIDED,
//                               {
//                                   {addr0, old0, new0},
//                               }};
//   return casn(&casn_desc);
// }
//
// uint64_t dcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
//               uint64_t* addr1, uint64_t old1, uint64_t new1) {
//   CASNDescriptor casn_desc = {2,
//                               UNDECIDED,
//                               {
//                                   {addr0, old0, new0},
//                                   {addr1, old1, new1},
//                               }};
//   return casn(&casn_desc);
// }
//
// uint64_t tcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
//               uint64_t* addr1, uint64_t old1, uint64_t new1,
//               uint64_t* addr2, uint64_t old2, uint64_t new2) {
//   CASNDescriptor casn_desc = {3,
//                               UNDECIDED,
//                               {
//                                   {addr0, old0, new0},
//                                   {addr1, old1, new1},
//                                   {addr2, old2, new2},
//                               }};
//   return casn(&casn_desc);
// }
//
// uint64_t qcas(uint64_t* addr0, uint64_t old0, uint64_t new0,
//               uint64_t* addr1, uint64_t old1, uint64_t new1,
//               uint64_t* addr2, uint64_t old2, uint64_t new2,
//               uint64_t* addr3, uint64_t old3, uint64_t new3) {
//   CASNDescriptor casn_desc = {4,
//                               UNDECIDED,
//                               {
//                                   {addr0, old0, new0},
//                                   {addr1, old1, new1},
//                                   {addr2, old2, new2},
//                                   {addr3, old3, new3},
//                               }};
//   return casn(&casn_desc);
// }
//
// uint64_t cas_read(uint64_t* addr) {
//   return casn_read(addr);
// }

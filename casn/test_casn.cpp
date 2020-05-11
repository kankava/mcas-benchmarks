#include "casn.h"
#include <cstdio>
#include <cstdint>

void test_get_rdcss_descriptor() {
    printf("Testing GetRDCSSDescriptor:\n");
    uint64_t data[] = {0, 0};

    RDCSSDescriptor d;
    d.a1 = &data[0];
    d.o1 = 0;
    d.a2 = &data[1];
    d.o2 = 0;
    d.n2 = 1;

    if (&d != get_rdcss_descriptor(get_rdcss_descriptor_ptr(&d))) {
        printf("    [FAILED] GetRDCSSDescriptor\n");
    } else {
        printf("    [SUCCESS] GetRDCSSDescriptor\n");
    }
}

void test_rdcss() {
    uint64_t control = 0;
    uint64_t data = 0;

    RDCSSDescriptor d;
    d.a1 = &control;
    d.o1 = 0;
    d.a2 = &data;
    d.o2 = 0;
    d.n2 = 1;

    uint64_t old = rdcss(&d);

    if ((old != 0) && (data != 1)) {
        printf("    [FAILED] RDCSS\n");
    } else {
        printf("    [SUCCESS] RDCSS\n");
    }
}


void test_casn() {
    uint64_t data[] = {0, 1, 2, 3};

    CASNDescriptor casn_desc = {4, UNDECIDED, {{&data[0], 0, 1}, 
                                               {&data[1], 1, 2},
                                               {&data[2], 2, 3},
                                               {&data[3], 3, 4}
                                              }
                               };

    if (casn(&casn_desc) != true) {
        printf("    [FAILED] CASN should be successful\n");
    } else {
        printf("    [SUCCESS] CASN\n");
    }

    if ((data[0] != 1) || (data[1] != 2) || (data[2] != 3 || data[3] != 4)) {
        printf("    [FAILED] CASN didn't swap values\n");
    } else {
        printf("    [SUCCESS] CASN value swap\n");
    }

    CASNDescriptor casn_desc2 = {4, UNDECIDED, {{&data[0], 0, 1}, 
                                             {&data[1], 1, 2},
                                             {&data[2], 2, 3},
                                             {&data[3], 3, 4}
                                            }
                                };
    
    if (casn(&casn_desc2) != false) {
        printf("    [FAILED] CASN should have failed\n");
    } else {
        printf("    [SUCCESS] CASN\n");
    }

    if ((data[0] != 1) || (data[1] != 2) || (data[2] != 3 || data[3] != 4)) {
        printf("    [FAILED] CASN shouldn't have swapped values\n");
    } else {
        printf("    [SUCCESS] CASN\n");
    }

}

void test_casn_par() {
    uint64_t data[] = {0, 1, 2, 3};

    #pragma omp parallel for
    for (uint64_t i = 0; i < 10; i++) {
        CASNDescriptor casn_desc = 
            {4, UNDECIDED, {{&data[0], i + 0, i + 1}, 
                            {&data[1], i + 1, i + 2},
                            {&data[2], i + 2, i + 3},
                            {&data[3], i + 3, i + 4}
                           }
            };
        casn(&casn_desc);
    }

    printf("%lu %lu %lu %lu\n", data[0], data[1], data[2], data[3]);
}

int main() {
    test_get_rdcss_descriptor();
    test_rdcss();
    test_casn();
    test_casn_par();
    return 0;
}

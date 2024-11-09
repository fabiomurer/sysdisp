#include <stdio.h>

void __attribute__((constructor)) run_me_first1() {
    printf("I run first1\n");
}

void __attribute__((constructor)) run_me_first2() {
    printf("I run first2\n");
}
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("hi from simpleprog\n");
    execlp("./childprog", "./chlidprog", NULL);
    return 0;
}
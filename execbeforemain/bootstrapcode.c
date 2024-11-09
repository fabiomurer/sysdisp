#include <stdio.h>


/*
https://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Function-Attributes.html
constructor
destructor
constructor (priority)
destructor (priority)
    The constructor attribute causes the function to be called automatically before execution enters main (). Similarly, the destructor attribute causes the function to be called automatically after main () has completed or exit () has been called. Functions with these attributes are useful for initializing data that will be used implicitly during the execution of the program.

    You may provide an optional integer priority to control the order in which constructor and destructor functions are run. A constructor with a smaller priority number runs before a constructor with a larger priority number; the opposite relationship holds for destructors. So, if you have a constructor that allocates a resource and a destructor that deallocates the same resource, both functions typically have the same priority. The priorities for constructor and destructor functions are the same as those specified for namespace-scope C++ objects (see C++ Attributes).

    These attributes are not currently implemented for Objective-C. 
*/
void __attribute__((constructor)) run_me_first1() {
    printf("I run first1\n");
}

void __attribute__((constructor)) run_me_first2() {
    printf("I run first2\n");
}
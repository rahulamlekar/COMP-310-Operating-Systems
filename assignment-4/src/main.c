#include <string.h>
#include "malloc/memory_allocation.h"

int main() {
    void* test1 = my_malloc(54);
    void* test2 = my_malloc(21);
    void* test3 = my_malloc(51);

    // Delete the contents, and see if stuff gets messed up
    memset(test2, '\0', 21);
    memset(test3, '\0', 51);

    void* test4 = my_malloc(81);
    void* test5 = my_malloc(27);

    // Try to free
    my_free(test4);
    void* test6 = my_malloc(41);


    return 0;
}
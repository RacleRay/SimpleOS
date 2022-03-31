#include <common/print.h>
#include <common/types.h>


typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors() {
    for (constructor* i = &start_ctors; i != &end_ctors; ++i) {
        (*i)();
    }
}

extern "C" void kernelMain(void* multiboot_struct, uint32_t magic_num) {
    printf((const char*)"hello os!\n");
    while (1) {
    };
}
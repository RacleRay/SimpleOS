#include <common/print.h>
#include <common/types.h>
#include <interrupt/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/handlers.h>


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

    GlobalDescriptorTable gdt;
    // HardwareInterruptOffset: 0x20, where the addr of interrupt from CPU timer is.
    InterruptManager interrupts(0x20, &gdt, nullptr);
    printf("Initializing hardware, stage 1\n");

    DriverManager drvManager;

    PrintKeyboardEventHandler kbhandler;
    KeyBoardDriver keyboard(&interrupts, &kbhandler);
    drvManager.AddDriver(&keyboard);

    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);
    drvManager.AddDriver(&mouse);

    printf("Initializing hardware, stage 2\n");
    drvManager.ActivateAll();

    printf("Initializing hardware, stage 3\n");
    interrupts.Activate();

    while (1) {
    };
}
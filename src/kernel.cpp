#include <common/print.h>
#include <common/types.h>
#include <interrupt/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/handlers.h>
#include <io/pci.h>


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

    PeripheralComponentInterconnectController PCIController;
    PCIController.SelectDrivers(&drvManager, &interrupts);

    printf("Initializing hardware, stage 2\n");
    drvManager.ActivateAll();

    printf("Initializing hardware, stage 3\n");
    interrupts.Activate();

    VideoGraphicsArray vga;
    vga.SetMode(320, 200, 0);
    // set all the screen to be blue.
    vga.FillRectangle(0, 0, 320, 200, 0x00, 0x00, 0xAB);
    // for (int32_t y = 0; y < 200; y++) {
    //     for (int32_t x = 0; x < 320; x++) {
    //         vga.PutPixel(x, y, 0x00, 0x00, 0xa8);
    //     }
    // }

    while (1) {
    };
}
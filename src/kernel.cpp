#include <common/print.h>
#include <common/types.h>
#include <interrupt/interrupts.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/handlers.h>
#include <io/pci.h>
#include <gui/desktop.h>
#include <gui/window.h>

#define GRAPHICMODE

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

#ifdef GRAPHICMODE
    Desktop desktop(320, 200, 0x00, 0x00, 0xa8);
#endif

#ifdef GRAPHICMODE
    KeyBoardDriver keyboard(&interrupts, &desktop);
#else
    PrintKeyboardEventHandler kbhandler;
    KeyBoardDriver keyboard(&interrupts, &kbhandler);
#endif
    drvManager.AddDriver(&keyboard);

#ifdef GRAPHICMODE
    MouseDriver mouse(&interrupts, &desktop);
#else
    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);
#endif
    drvManager.AddDriver(&mouse);

    PeripheralComponentInterconnectController PCIController;
    PCIController.SelectDrivers(&drvManager, &interrupts);
    VideoGraphicsArray vga;

    printf("Initializing hardware, stage 2\n");
    drvManager.ActivateAll();

#ifdef GRAPHICMODE
    vga.SetMode(320, 200, 0);
    // set all the screen to be blue. （Done by desktop init. #line 32）
    // vga.FillRectangle(0, 0, 320, 200, 0x00, 0x00, 0xAB);
    // for (int32_t y = 0; y < 200; y++) {
    //     for (int32_t x = 0; x < 320; x++) {
    //         vga.PutPixel(x, y, 0x00, 0x00, 0xa8);
    //     }
    // }
    Window win1(&desktop, 10, 10, 20, 20, 0xAB, 0x00, 0x00);
    desktop.AddChild(&win1);
    Window win2(&desktop, 40, 20, 40, 40, 0x00, 0xAB, 0x00);
    desktop.AddChild(&win2);
#endif

    printf("Initializing hardware, stage 3\n");
    interrupts.Activate();

    // desktop.Draw 应该是交给显卡来做，而不是手动刷新。可以防止窗口闪烁的问题。
    while (1) {
#ifdef GRAPHICMODE
        desktop.Draw(&vga);
#endif
    };
}
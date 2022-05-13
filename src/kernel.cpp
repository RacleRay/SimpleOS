#include <common/print.h>
#include <common/types.h>
#include <interrupt/interrupts.h>
#include <interrupt/multitasks.h>
#include <interrupt/syscalls.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/handlers.h>
#include <io/pci.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <memory/heapmanager.h>


// #define GRAPHICMODE

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors() {
    for (constructor* i = &start_ctors; i != &end_ctors; ++i) {
        (*i)();
    }
}

extern "C" void taskA() {
    printf("AAA");
}

extern "C" void taskB() {
    printf("BBB");
}

extern "C" void kernelMain(void* multiboot_struct, uint32_t magic_num) {
    printf((const char*)"hello os!\n");

    GlobalDescriptorTable gdt;

    // multiboot_struct：linux 系统实现 http://www.gnu.org/software/grub/manual/multiboot/html_node/multiboot_002eh.html#multiboot_002eh
    // An OS image must contain an additional header called Multiboot header, besides the headers of the format used by the OS image.
    // The Multiboot header must be contained completely within the first 8192 bytes of the OS image, and must be longword (32-bit) aligned.
    size_t heap = 10 * 1024 * 1024;
    // + 8 表示占位，参考 linux 系统中 multiboot_struct 的实现
    uint32_t* memupper = (uint32_t*)((size_t)multiboot_struct + 8);
    // for debug
    printfHex(((*memupper) >> 24) & 0xff);
    printfHex(((*memupper) >> 16) & 0xff);
    printfHex(((*memupper) >> 8) & 0xff);
    printfHex(((*memupper) >> 0) & 0xff);

    MemoryManager memoryManager(heap, (*memupper) * 1024 - heap - 10 * 1024);

    // for debug
    printf("\n heap: 0x");
    printfHex((heap >> 24) & 0xff);
    printfHex((heap >> 16) & 0xff);
    printfHex((heap >> 8) & 0xff);
    printfHex((heap >> 0) & 0xff);

    TaskManger taskManger;
    // Task task1(&gdt, taskA);
    // Task task2(&gdt, taskB);
    // taskManger.AddTask(&task1);
    // taskManger.AddTask(&task2);

    // HardwareInterruptOffset: 0x20, where the addr of interrupt from CPU timer is.
    InterruptManager interrupts(0x20, &gdt, &taskManger);
    // InterruptManager interrupts(0x20, &gdt, nullptr);

    SyscallHandler syscalls(&interrupts, 0x80);

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
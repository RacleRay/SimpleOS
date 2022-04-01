#include <drivers/handlers.h>


void PrintKeyboardEventHandler::OnKeyDown(char c) {
    char* foo = (char*)" ";
    foo[0]    = c;
    printf(foo);
}


void MouseToConsole::OnActivate() {
    uint16_t* VideoMemory   = (uint16_t*)0xb8000;
    VideoMemory[y * 80 + x] = ((VideoMemory[y * 80 + x] & 0xf000) >> 4)
                              | ((VideoMemory[y * 80 + x] & 0x0f00) << 4)
                              | (VideoMemory[y * 80 + x] & 0x00ff);
}

void MouseToConsole::OnMouseMove(int8_t nx, int8_t ny) {
    uint16_t* VideoMemory   = (uint16_t*)0xb8000;
    VideoMemory[y * 80 + x] = ((VideoMemory[y * 80 + x] & 0xf000) >> 4)
                              | ((VideoMemory[y * 80 + x] & 0x0f00) << 4)
                              | (VideoMemory[y * 80 + x] & 0x00ff);

    x += nx;
    if (x < 0)
        x = 0;
    else if (x >= 80)
        x = 79;

    y += ny;
    if (y < 0)
        y = 0;
    else if (y >= 25)
        y = 24;

    VideoMemory[y * 80 + x] = ((VideoMemory[y * 80 + x] & 0xf000) >> 4)
                              | ((VideoMemory[y * 80 + x] & 0x0f00) << 4)
                              | (VideoMemory[y * 80 + x] & 0x00ff);
}

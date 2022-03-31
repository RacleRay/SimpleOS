#include <common/print.h>

#define TERMHEIGHT 80
#define TERMWIDTH 25

void printf(const char* str) {
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    // 屏幕内容输出是从 VideoMemory 中读取的
    // 设置输出格式和多行输出
    static uint8_t x = 0, y = 0;
    for (int i = 0; str[i]; ++i) {
        switch (str[i]) {
        case '\n':
            y++;
            x = 0;
            break;
        default:
            VideoMemory[TERMHEIGHT * y + x] =
                (VideoMemory[TERMHEIGHT * y + x] & 0xFF00) | str[i];
            x++;
            break;
        }

        if (x >= TERMHEIGHT) {
            x = 0;
            y++;
        }

        if (y >= TERMWIDTH) {
            for (y = 0; y < TERMWIDTH; y++) {
                for (x = 0; x < TERMHEIGHT; x++) {
                    VideoMemory[TERMHEIGHT * y + x] =
                        (VideoMemory[TERMHEIGHT * y + x] & 0xFF00) | ' ';
                }
            }
            x = 0, y = 0;
        }
    }
}

void printfHex(uint8_t key) {
    char*       foo = (char*)"00";
    const char* hex = "0123456789ABCDEF";
    foo[0]          = hex[(key >> 4) & 0x0f];
    foo[1]          = hex[key & 0x0f];
    printf((const char*)foo);
}

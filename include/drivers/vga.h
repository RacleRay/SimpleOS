#ifndef __DRIVERS__VGA_H
#define __DRIVERS__VGA_H

#include "common/types.h"
#include "io/port.h"
#include "drivers/driver.h"

/*
VGA 在BIOS启动是，通过0x13中断，进入Graph mode，向屏幕输出内容。
但是在此处在虚拟机内实现OS，不能进入到BIOS，这里是直接向 VGA 输出。
VGA 从固定内存位置读取字符，所以这里直接向那个内存地址写数据，就能输出到屏幕。
这里 VGA 没有通过PCI来通信。

WARNING: 运行可能会出错，可能系统崩溃。VGA输入是标准化的，如果输入出错，尤其是
在操作系统之上的虚拟机环境下，可能出现不可预知的行为。
*/
class VideoGraphicsArray {
public:
    VideoGraphicsArray();
    ~VideoGraphicsArray();

    // check mode. Mode is represented by 'width, height, colordepth'
    bool SupportsMode(uint32_t width, uint32_t height, uint32_t colordepth);
    bool SetMode(uint32_t width, uint32_t height, uint32_t colordepth);
    // coordinates: x, y  color: r, g, b
    void PutPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b);
    void PutPixel(int32_t x, int32_t y, uint8_t colorindex);

    void FillRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r,
        uint8_t g, uint8_t b);
private:
    Port8Bit miscPort;
    Port8Bit crtcIndexPort;
    Port8Bit crtcDataPort;
    Port8Bit sequencerIndexPort;
    Port8Bit sequencerDataPort;
    Port8Bit graphicsControllerIndexPort;
    Port8Bit graphicsControllerDataPort;
    Port8Bit attributeControllerIndexPort;
    Port8Bit attributeControllerReadPort;
    Port8Bit attributeControllerWritePort;
    Port8Bit attributeControllerResetPort;

    // send initializatioin codes to responding ports.
    void WriteRegisters(uint8_t* registers);
    // 每种颜色code只在内存中储存一份，只需要找到需要的颜色位置，
    uint8_t GetColorIndex(uint8_t r, uint8_t g, uint8_t b);
    // get memory address to read and write.
    uint8_t* GetFrameBufferSegment();
};


#endif
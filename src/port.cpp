#include <io/port.h>

/*
Port:
    - 当键盘输入，传到 PIC(可编程中断控制器 (Programmable Interrupt Controller))
    - PIC 将数据传递到连接着各种设备的 multiplexer
    - 根据 port number 将数据传输到不同的设备
    - 设备返回的数据也由该 port 返回到 multiplexer 进行传输

注意：Port传输数据的 bandwidth 带宽，如果是 32bit 可以兼容 16bit 8bit 的数据，
     反之，编译器会报错的，不用担心运行时出现这样的情况
*/

Port::Port(uint16_t portnumber) : portnumber(portnumber) {}

Port::~Port() {}

// ###############################################################
// 8 bits version
Port8Bit::Port8Bit(uint16_t portnumber) : Port(portnumber) {}

Port8Bit::~Port8Bit() {}

void Port8Bit::Write(uint8_t data) { Write8(portnumber, data); }

uint8_t Port8Bit::Read() { return Read8(portnumber); }

Port8BitSlow::Port8BitSlow(uint16_t portnumber) : Port8Bit(portnumber) {}

Port8BitSlow::~Port8BitSlow() {}

void Port8BitSlow::Write(uint8_t data) { Write8Slow(portnumber, data); }

// ###############################################################
// 16 bits version
Port16Bit::Port16Bit(uint16_t portnumber) : Port(portnumber) {}

Port16Bit::~Port16Bit() {}

void Port16Bit::Write(uint16_t data) { Write16(portnumber, data); }

uint16_t Port16Bit::Read() { return Read16(portnumber); }

// ###############################################################
// 32 bits version
Port32Bit::Port32Bit(uint16_t portnumber) : Port(portnumber) {}

Port32Bit::~Port32Bit() {}

void Port32Bit::Write(uint32_t data) { Write32(portnumber, data); }

uint32_t Port32Bit::Read() { return Read32(portnumber); }
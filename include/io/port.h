#ifndef __HARDWAREIO__PORT_H
#define __HARDWAREIO__PORT_H

#include "common/types.h"


class Port {
protected:
    uint16_t portnumber;
    Port(uint16_t portnumber);
    ~Port();
};

class Port8Bit : public Port {
public:
    Port8Bit(uint16_t portnumber);
    ~Port8Bit();
    virtual void    Write(uint8_t data);
    virtual uint8_t Read();

protected:
    // read 8 bits data from port (_port).
    static inline uint8_t Read8(uint16_t _port) {
        uint8_t result;
        __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(_port));
        return result;
    }
    // write 8 bits data to port (_port).
    static inline void Write8(uint16_t _port, uint8_t _data) {
        __asm__ volatile("outb %0, %1" : : "a"(_data), "Nd"(_port));
    }
};

// Inherit
class Port8BitSlow : public Port8Bit {
public:
    Port8BitSlow(uint16_t portnumber);
    ~Port8BitSlow();
    virtual void Write(uint8_t data);

protected:
    // slow: add some operation (jmp) to wait a little until the port is done
    //       writing the data
    static inline void Write8Slow(uint16_t _port, uint8_t _data) {
        __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:"
                         :
                         : "a"(_data), "Nd"(_port));
    }
};

class Port16Bit : public Port {
public:
    Port16Bit(uint16_t portnumber);
    ~Port16Bit();
    virtual void     Write(uint16_t data);
    virtual uint16_t Read();

protected:
    static inline uint16_t Read16(uint16_t _port) {
        uint16_t result;
        __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(_port));
        return result;
    }

    static inline void Write16(uint16_t _port, uint16_t _data) {
        __asm__ volatile("outw %0, %1" : : "a"(_data), "Nd"(_port));
    }
};

class Port32Bit : public Port {
public:
    Port32Bit(uint16_t portnumber);
    ~Port32Bit();
    virtual void     Write(uint32_t data);
    virtual uint32_t Read();

protected:
    static inline uint32_t Read32(uint16_t _port) {
        uint32_t result;
        __asm__ volatile("inl %1, %0" : "=a"(result) : "Nd"(_port));
        return result;
    }

    static inline void Write32(uint16_t _port, uint32_t _data) {
        __asm__ volatile("outl %0, %1" : : "a"(_data), "Nd"(_port));
    }
};

#endif
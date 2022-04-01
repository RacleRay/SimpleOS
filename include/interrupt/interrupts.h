/*
PIC 查询 IDT(Interrupt descriptor table) 获取相应中断信息

IDT 中保存有各种中断对应的处理函数handler以及一些参数 flags,access code等

由于这些需要在内核空间执行，所以不用C++函数实现，而是通过 loader.s ，
借助 assembler 实现。
*/

#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include <common/types.h>
#include <io/port.h>
#include <memory/gdt.h>
#include <interrupt/multitasks.h>


class InterruptManager;

class InterruptHandler {
public:
    /**
     * @brief interrupt to another cpu task.
     *
     * @param esp -- current stack pointer
     * @return uint32_t -- other cpu task stack pointer
     */
    virtual uint32_t HandleInterrupt(uint32_t esp);

protected:
    InterruptHandler(uint8_t           interruptNumber,
                     InterruptManager* interruptManager);
    ~InterruptHandler();

    uint8_t           interruptNumber;
    InterruptManager* interruptManager;
};


class InterruptManager {
    friend class InterruptHandler;

public:
    InterruptManager(uint16_t               hardwareInterruptOffset,
                     GlobalDescriptorTable* gdt,
                     TaskManger*            taskManger);
    ~InterruptManager();

    uint16_t HardwareInterruptOffset();
    void     Activate();
    void     Deactivate();

protected:
    /* 指向当前示例化唯一的 InterruptManager。 */
    static InterruptManager* ActiveInterruptManager;
    InterruptHandler*        handlers[256];
    TaskManger*              taskManger;

    /* IDT(Interrupt descriptor table) 中表项 */
    struct GateDescriptor {
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegmentSelector;
        uint8_t  reserved;
        uint8_t  access;
        uint16_t handlerAddressHighBits;
    } __attribute__((packed));

    // IDT
    static GateDescriptor interruptDescriptorTable[256];

    // IDT 表项设置
    static void SetInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t  DescriptorPrivilegelLevel,
        uint8_t DescriptorType
    );

    // 提供给 CPU 的 IDT 地址
    struct InterruptDescriptorTablePointer {
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));

    uint16_t hardwareInterruptOffset;

    // default interrupt handler：在 interruptstubs.s 中定义
    // 定义时的函数名称，可以先 make，报错信息会提示缺少哪个函数的定义
    static void InterruptIgnore();

    // 以下为中断处理函数
    static uint32_t HandleInterrupt(uint8_t interruptNumber, uint32_t esp);
    uint32_t DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp);

    static void HandleInterruptRequest0x00();
    static void HandleInterruptRequest0x01();
    static void HandleInterruptRequest0x02();
    static void HandleInterruptRequest0x03();
    static void HandleInterruptRequest0x04();
    static void HandleInterruptRequest0x05();
    static void HandleInterruptRequest0x06();
    static void HandleInterruptRequest0x07();
    static void HandleInterruptRequest0x08();
    static void HandleInterruptRequest0x09();
    static void HandleInterruptRequest0x0A();
    static void HandleInterruptRequest0x0B();
    static void HandleInterruptRequest0x0C();
    static void HandleInterruptRequest0x0D();
    static void HandleInterruptRequest0x0E();
    static void HandleInterruptRequest0x0F();
    static void HandleInterruptRequest0x31();

    static void HandleException0x00();
    static void HandleException0x01();
    static void HandleException0x02();
    static void HandleException0x03();
    static void HandleException0x04();
    static void HandleException0x05();
    static void HandleException0x06();
    static void HandleException0x07();
    static void HandleException0x08();
    static void HandleException0x09();
    static void HandleException0x0A();
    static void HandleException0x0B();
    static void HandleException0x0C();
    static void HandleException0x0D();
    static void HandleException0x0E();
    static void HandleException0x0F();
    static void HandleException0x10();
    static void HandleException0x11();
    static void HandleException0x12();
    static void HandleException0x13();

    // PIC port ：交互 interrupt 数据的port, 分为 data 和 command
    Port8BitSlow picMasterCommand;
    Port8BitSlow picMasterData;
    Port8BitSlow picSlaveCommand;
    Port8BitSlow picSlaveData;
};

#endif
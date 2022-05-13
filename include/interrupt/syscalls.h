#ifndef __SYSCALLS_H
#define __SYSCALLS_H

#include "common/types.h"
#include "interrupt/interrupts.h"
#include "interrupt/multitasks.h"


// 用户调用内核方法的中介
class SyscallHandler : public InterruptHandler {
public:
    SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber);
    ~SyscallHandler();

    virtual uint32_t HandleInterrupt(uint32_t esp);
};

#endif
#include "common/print.h"
#include "interrupt/syscalls.h"


SyscallHandler::SyscallHandler(InterruptManager* interruptManager,
                               uint8_t           InterruptNumber)
    : InterruptHandler(InterruptNumber + interruptManager->HardwareInterruptOffset(),
                       interruptManager) {}

SyscallHandler::~SyscallHandler() {}


uint32_t SyscallHandler::HandleInterrupt(uint32_t esp) {
    CPUState* cpu = (CPUState*)esp;

    // example code, to deal with different system calls.
    switch (cpu->eax) {
    case 4:
        printf((char*)cpu->ebx);
        break;
    default:
        break;
    }

    return esp;
}
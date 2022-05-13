
#ifndef __MULTITASKING_H
#define __MULTITASKING_H

#include <common/types.h>
#include <memory/gdt.h>

struct CPUState {
    // Pushed in the interrupt steps:
    // accumulator, base register, counting register, data register,
    // stack index, data index, stack base pointer.
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp;

    uint32_t error;  // error code

    // Pushed by processor:
    // instruction pointer, code segement, flags,
    // stack pointer, stack segement
    uint32_t eip, cs, eflags, esp, ss;
} __attribute__((packed));


// 用于在不同内存地址，执行不同的代码任务
// 这里的任务存在于用户空间
class Task {
    friend class TaskManger;

public:
    Task(GlobalDescriptorTable* gdt, void entrypoint());
    ~Task();

private:
    uint8_t   stack[4096];
    CPUState* cpustate;
};


// 任务管理调度
class TaskManger {
public:
    TaskManger();
    ~TaskManger();
    bool      AddTask(Task* task);
    CPUState* Schedule(CPUState* cpustate);

private:
    Task* tasks[256];
    int   numTasks;
    int   currentTask;
};

#endif
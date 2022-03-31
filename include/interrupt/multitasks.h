
#ifndef __MULTITASING_H
#define __MULTITASING_H

#include <common/types.h>
#include <memory/gdt.h>

struct CPUState {
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp;
    uint32_t error, eip, cs, eflags, esp, ss;
} __attribute__((packed));


class Task {
    friend class TaskManger;

public:
    Task(GlobalDescriptorTable* gdt, void entrypoint());
    ~Task();

private:
    uint8_t   stack[4096];
    CPUState* cpustate;
};


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
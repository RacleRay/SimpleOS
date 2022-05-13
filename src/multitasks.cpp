#include <interrupt/multitasks.h>

Task::Task(GlobalDescriptorTable* gdt, void entrypoint()) {
    // phony entry: cpu state 初始化在高地址，Task栈空间的尾端
    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));

    cpustate->eax = 0;
    cpustate->ebx = 0;
    cpustate->ecx = 0;
    cpustate->edx = 0;

    cpustate->esi = 0;
    cpustate->edi = 0;
    cpustate->ebp = 0;

    cpustate->eip    = (uint32_t)entrypoint;
    cpustate->cs     = gdt->CodeSegmentSelector() << 3;
    cpustate->eflags = 0x202;  // 0010 0000 0010
}


TaskManger::TaskManger() : numTasks(0), currentTask(-1) {}

bool TaskManger::AddTask(Task* task) {
    if (numTasks >= 256) {
        return false;
    }
    tasks[numTasks++] = task;
    return true;
}

// 通过中断调度不同tasks，见 interrupts.h 中 DoHandleInterrupt 方法实现。
CPUState* TaskManger::Schedule(CPUState* cpustate) {
    if (numTasks <= 0) return cpustate;

    if (currentTask >= 0) {
        tasks[currentTask]->cpustate = cpustate;
    }

    if (++currentTask >= numTasks) {
        currentTask %= numTasks;
    }
    return tasks[currentTask]->cpustate;
}
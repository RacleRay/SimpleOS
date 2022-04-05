/*
Peripheral Component Interconnect (PCI)

统一外部设备的交互。数据传输时，由一个PCI设备做发起者，而另一个PCI设备做目标。
CPU调度的driver程序，通过 PCI 找到对应的物理设备进行通信。

此系统实现的PCI有最多有8个bus总线，每条bus最多32个device连接。每个device连接有8个function，负责不同的功能.
所以需要 3bits(8) + 5bits(32) + 3bits(8) 作为设备接口的地址表示。
*/

#ifndef __IO__PCI_H
#define __IO__PCI_H

#include "common/types.h"
#include "drivers/driver.h"
#include "interrupt/interrupts.h"
#include "io/port.h"


// BaseAddressRegister两种类型
// InputOutput: 无缓存，流式读取数据方式。比如与键盘的port进行传输的时候，用这个
// MemoryMapping: prefetch模式。数据先被缓存到MemoryMapping空间的某个位置。
//                  然后通知相应的中断程序进行读取。这样显然可以在prefetch准备
//                  数据的时候，让CPU进行其他任务，效率提升。
enum BaseAddressRegisterType {
    MemoryMapping = 0,
    InputOutput = 1
};

// PCI作为CPU和设备之间的数据中转，告诉CPU在哪个位置读，设备数据往哪写。
class BaseAddressRegister {
public:
    bool                    prefetchable;
    uint8_t*                address;
    uint32_t                size;
    BaseAddressRegisterType type;
};


// 物理设备接口信息
// Linux command:  lspci -n  可以查看机器的物理设备的信息（虚拟机上也行）
class PeripheralComponentInterconnectDeviceDescriptor {
public:
    PeripheralComponentInterconnectDeviceDescriptor();
    ~PeripheralComponentInterconnectDeviceDescriptor();

    uint32_t portBase;
    uint32_t interrupt;

    uint8_t bus;
    uint8_t device;
    uint8_t function;

    uint16_t device_id;
    uint16_t vendor_id;

    uint8_t class_id;
    uint8_t subclass_id;
    uint8_t interface_id;
    uint8_t revision;
};


// http://www.lowlevel.eu/wiki/Peripheral_Component_Interconnect
class PeripheralComponentInterconnectController {
private:
    Port32Bit dataPort;  // 通信 port 数据和指令
    Port32Bit commandPort;

public:
    PeripheralComponentInterconnectController();
    ~PeripheralComponentInterconnectController();

    // bus, device, function: 找到物理设备接口地址
    // registeroffset: read找到设备内存位置，再由offset，找到需要读取的数据地址
    uint32_t Read(uint8_t bus, uint8_t device, uint8_t function,
                  uint8_t registeroffset);

    void write(uint8_t bus, uint8_t device, uint8_t function,
               uint8_t registeroffset, uint32_t value);

    // 避免轮询一个device下的每个 function，直接查看设备下有无可用 function
    bool DeviceHasFunctions(uint8_t bus, uint8_t device);

    // 找到对应物理设备的driver，并注册到 driverManager ，通过中断调用相应的回调函数实现各种功能
    void SelectDrivers(DriverManager*    driverManager,
                       InterruptManager* interrupts);

    PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(uint8_t bus,
                                                                        uint8_t device,
                                                                        uint8_t function);

    // 
    Driver* GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev,
                      InterruptManager* interrupts);

    BaseAddressRegister GetBaseAddressRegister(uint8_t bus, uint8_t device,
                                               uint8_t function, uint8_t bar);
};


#endif
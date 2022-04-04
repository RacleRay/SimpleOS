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


enum BaseAddressRegisterType {
    MemoryMapping = 0,
    InputOutput = 1
};


class BaseAddressRegister {
public:
    bool                    prefetchable;
    uint8_t*                address;
    uint32_t                size;
    BaseAddressRegisterType type;
};


// 物理设备接口信息
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

    Driver* GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev,
                      InterruptManager* interrupts);

    BaseAddressRegister GetBaseAddressRegister(uint8_t bus, uint8_t device,
                                               uint8_t function, uint8_t bar);
};


#endif
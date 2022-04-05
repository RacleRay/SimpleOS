#include "io/pci.h"
#include "common/print.h"

// ============== PeripheralComponentInterconnectDeviceDescriptor ==============
PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor() {}
PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor() {}


// ============== PeripheralComponentInterconnectController ==============
PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
    : dataPort(0xcfc),
    commandPort(0xcf8) {}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController() {}

uint32_t PeripheralComponentInterconnectController::Read(uint8_t bus,
    uint8_t device,
    uint8_t function,
    uint8_t registeroffset) {
    uint32_t id =
        1 << 31 |
        ((bus & 0xff) << 16) |
        ((device & 0x1f) << 11) |
        ((function & 0x07) << 8) |
        (registeroffset & 0xfc);
    commandPort.Write(id);
    uint32_t result = dataPort.Read();
    return result >> (8 * (registeroffset % 4));
}

void PeripheralComponentInterconnectController::write(uint8_t bus,
    uint8_t device,
    uint8_t function,
    uint8_t registeroffset,
    uint32_t value) {
    uint32_t id =
        1 << 31 |
        ((bus & 0xff) << 16) |
        ((device & 0x1f) << 11) |
        ((function & 0x07) << 8) |
        (registeroffset & 0xfc);
    commandPort.Write(id);
    dataPort.Write(value);
}

// 避免轮询一个device下的每个 function，直接查看设备下有无可用 function
bool PeripheralComponentInterconnectController::DeviceHasFunctions(uint8_t bus, uint8_t device) {
    return Read(bus, device, 0, 0x0e) & (1 << 7);
}

// 找到对应物理设备的driver，并注册到 driverManager ，通过中断调用相应的回调函数实现各种功能
void PeripheralComponentInterconnectController::SelectDrivers(DriverManager* driverManager, InterruptManager* interrupts) {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            int numFunctions = DeviceHasFunctions((uint8_t)bus, device) ? 8 : 1;
            for (uint8_t function = 0; function < numFunctions; function++) {
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);
                if (dev.vendor_id == 0 || dev.vendor_id == 0xffff) continue;

                printf("PCI BUS ");
                printfHex(bus & 0xff);

                printf(", DEVICE ");
                printfHex(device);

                printf(", FUNCTION ");
                printfHex(function);

                printf(" = VENDOR ");
                printfHex((dev.vendor_id & 0xff00) >> 8);
                printfHex(dev.vendor_id & 0xff);

                printf(", DEVICE ");
                printfHex((dev.device_id & 0xff00) >> 8);
                printfHex(dev.device_id & 0xff);
                printf("\n");

                for (uint8_t barNum = 0; barNum < 6; barNum++) {
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);
                    if (bar.address && (bar.type == InputOutput)) {
                        dev.portBase = (uint32_t)bar.address;
                    }
                }

                Driver* driver = GetDriver(dev, interrupts);
                if (driver != 0) {
                    driverManager->AddDriver(driver);
                }
            }
        }
    }
}

// TODO
Driver* PeripheralComponentInterconnectController::GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev, InterruptManager* interrupts) {
    Driver* driver = 0;
    // 使用 hardcode 0x1022 这种。因为使用虚拟机，没法从设备driver中的文件读取。
    switch (dev.vendor_id) {
    case 0x1022: // AMD
        break;
    case 0x8086: // intel
        break;
    }

    switch (dev.class_id) {
    case 0x03:
        switch (dev.subclass_id) {
        case 0x00: // VGA
            printf("VGA ");
            break;
        }
        break;
    }
    return 0;
}

// 物理设备接口信息
PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(uint8_t bus, uint8_t device, uint8_t function) {
    PeripheralComponentInterconnectDeviceDescriptor result;

    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = Read(bus, device, function, 0);
    result.device_id = Read(bus, device, function, 0x02);

    result.class_id = Read(bus, device, function, 0x0b);
    result.subclass_id = Read(bus, device, function, 0x0a);
    result.interface_id = Read(bus, device, function, 0x09);
    result.revision = Read(bus, device, function, 0x08);

    result.interrupt = Read(bus, device, function, 0x3c);
    return result;
}


BaseAddressRegister PeripheralComponentInterconnectController::GetBaseAddressRegister(uint8_t bus, uint8_t device, uint8_t function, uint8_t bar) {
    BaseAddressRegister result;

    // headertype: 00 -- 表示32位  01 -- 表示20位  10 -- 表示64位
    uint32_t headertype = Read(bus, device, function, 0x0e) & 0x7e;
    int maxBARs = 6 - 4 * headertype;
    if (bar >= maxBARs) return result;  // illegal

    uint32_t bar_value = Read(bus, device, function, 0x10 + 4 * bar);
    result.type = (bar_value & 1) ? InputOutput : MemoryMapping;

    if (result.type == MemoryMapping) {
        // 具体实现见 http://www.lowlevel.eu/wiki/Peripheral_Component_Interconnect
        // 此处跳过
        switch ((bar_value >> 1) & 0x3) {
        case 0: // 32
            break;
        case 1: // 20
            break;
        case 2: // 64
            break;
        }
        result.prefetchable = ((bar_value >> 3) & 0x1) == 0x1;
    } else {
        // 屏蔽标志位
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = false;
    }
    return result;
}
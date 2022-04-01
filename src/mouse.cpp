#include "drivers/mouse.h"


MouseEventHandler::MouseEventHandler() {}
void MouseEventHandler::OnActivate() {}
void MouseEventHandler::OnMouseDown(uint8_t button) {}
void MouseEventHandler::OnMouseUp(uint8_t button) {}
void MouseEventHandler::OnMouseMove(int8_t x, int8_t y) {}


// InterruptHandler(...) 注册handler到interruptManager
// handler 为自定义的 MouseEventHandler 的子类对象
MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
    : InterruptHandler(0x0C + manager->HardwareInterruptOffset(), manager),
      dataport(0x60), commandport(0x64), offset(0), buttons(0),
      handler(handler) {}
MouseDriver::~MouseDriver() {}

// 与相应的 port 开始通信
void MouseDriver::Activate() {
    if (handler != nullptr) {
        handler->OnActivate();
    }

    commandport.Write(0xa8);  // active interrupt
    commandport.Write(0x20);  // get current state
    uint8_t status = (dataport.Read() | 2) & ~0x20;
    commandport.Write(0x60);  // set state
    dataport.Write(status);

    commandport.Write(0xd4);
    dataport.Write(0xf4);
    dataport.Read();
}

// 由 interruptManager 中 DoHandleInterrupt 调用
uint32_t MouseDriver::HandleInterrupt(uint32_t esp) {
    uint8_t status = commandport.Read();
    // status & 0x20: 是否有数据可读的标志位
    if (!(status & 0x20) || handler == nullptr) return esp;

    buffer[offset] = dataport.Read();
    offset         = (offset + 1) % 3;  // buffer: flag, x, y

    if (offset == 0) {
        handler->OnMouseMove(buffer[1], -buffer[2]);

        for (uint8_t i = 0; i < 3; i++) {
            if ((buffer[0] & (1 << i)) != (buttons & (1 << i))) {
                if (buttons & (1 << i)) {
                    handler->OnMouseUp(i + 1);
                }
                else {
                    handler->OnMouseDown(i + 1);
                }
            }
        }
        buttons = buffer[0];
    }

    return esp;
}
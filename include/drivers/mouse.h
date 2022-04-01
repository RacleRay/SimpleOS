#ifndef __DRIVERS__MOUSE_H
#define __DRIVERS__MOUSE_H

#include "common/types.h"
#include "drivers/driver.h"
#include "interrupt/interrupts.h"
#include "io/port.h"


// 自定义子类实现相应回调函数
class MouseEventHandler {
public:
    MouseEventHandler();
    virtual void OnActivate();
    virtual void OnMouseDown(uint8_t button);
    virtual void OnMouseUp(uint8_t button);
    virtual void OnMouseMove(int8_t x, int8_t y);
};

class MouseDriver : public InterruptHandler, public Driver {
public:
    MouseDriver(InterruptManager* manager, MouseEventHandler* handler);
    ~MouseDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
    virtual void     Activate();

private:
    Port8Bit dataport;
    Port8Bit commandport;

    uint8_t buffer[3];  // buffer: flag, x, y
    uint8_t offset;
    uint8_t buttons;

    int8_t             x, y;
    MouseEventHandler* handler;
};

#endif
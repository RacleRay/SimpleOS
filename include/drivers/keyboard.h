#ifndef __DRIVERS__KEYBOARD_H
#define __DRIVERS__KEYBOARD_H

#include "common/types.h"
#include "interrupt/interrupts.h"
#include "io/port.h"
#include "drivers/driver.h"


class KeyboardEventHandler {
public:
    KeyboardEventHandler();
    virtual void OnKeyDown(char);  // maybe use enum type designed by your self.
    virtual void OnKeyUp(char);
};

class KeyBoardDriver : public InterruptHandler, public Driver {
public:
    KeyBoardDriver(InterruptManager* manager, KeyboardEventHandler* handler);
    ~KeyBoardDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
    virtual void     Activate();

private:
    Port8Bit dataport;
    Port8Bit commandport;

    KeyboardEventHandler* handler;
};


#endif
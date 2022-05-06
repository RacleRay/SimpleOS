#ifndef __GUI_DESKTOP_H
#define __GUI_DESKTOP_H

#include "drivers/mouse.h"
#include "gui/widget.h"


class Desktop : public CompositeWidget, public MouseEventHandler {
public:
    Desktop(int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);
    ~Desktop();

    void Draw(GraphicsContext* gc) override;
    void OnMouseDown(uint8_t button) override;
    void OnMouseUp(uint8_t button) override;
    void OnMouseMove(int8_t x, int8_t y) override;

private:
    uint32_t MouseX, MouseY;
};


#endif
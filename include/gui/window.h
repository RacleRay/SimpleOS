#ifndef __GUI_WINDOW_H
#define __GUI_WINDOW_H

#include "drivers/mouse.h"
#include "gui/widget.h"


class Window : public CompositeWidget {
public:
    Window(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h,
           uint8_t r, uint8_t g, uint8_t b);
    ~Window();
    
    // 平板电脑之类的没有鼠标，显然不是普遍适用的操作系统。
    void OnMouseDown(int32_t x, int32_t y, uint8_t button) override;
    void OnMouseUp(int32_t x, int32_t y, uint8_t button) override;
    void OnMouseMove(int32_t x, int32_t y, int32_t nx, int32_t ny) override;

private:
    bool Dragging;
};


#endif
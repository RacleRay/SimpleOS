#ifndef __HANDLERS_H
#define __HANDLERS_H

#include <common/print.h>
#include <common/types.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

// 键盘输入，打印到screen
class PrintKeyboardEventHandler : public KeyboardEventHandler {
public:
    void OnKeyDown(char c);
};

// 光标随鼠标移动
class MouseToConsole : public MouseEventHandler {
public:
    MouseToConsole() : x(40), y(12) {}  // 初始位置

    void OnActivate();
    void OnMouseMove(int8_t xoffset, int8_t yoffset) override;

private:
    int8_t x, y;
};

#endif
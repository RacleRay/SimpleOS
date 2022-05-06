#ifndef __GUI_WIDGET_H
#define __GUI_WIDGET_H

#include "common/graphic.h"
#include "common/types.h"
#include "drivers/keyboard.h"


class Widget : public KeyboardEventHandler {
public:
    Widget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h,
           uint8_t r, uint8_t g, uint8_t b);
    ~Widget();

    virtual void GetFocus(Widget* widget);
    virtual void ModelToScreen(int32_t& x, int32_t& y);
    virtual bool ContainsCoordinate(int32_t x, int32_t y);

    virtual void Draw(GraphicsContext* gc);
    virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseMove(int32_t x, int32_t y, int32_t nx, int32_t ny);

protected:
    Widget* parent;
    int32_t x, y, w, h;
    uint8_t r, g, b;
    bool    Focussable;
};


class CompositeWidget : public Widget {
public:
    CompositeWidget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h,
                    uint8_t r, uint8_t g, uint8_t b);
    ~CompositeWidget();

    virtual void GetFocus(Widget* widget);
    virtual bool AddChild(Widget* child);

    virtual void Draw(GraphicsContext* gc) override;
    virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button) override;
    virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button) override;
    virtual void OnMouseMove(int32_t x, int32_t y, int32_t nx,
                             int32_t ny) override;

    virtual void OnKeyDown(char x) override;
    virtual void OnKeyUp(char x) override;

private:
    Widget* children[100];
    int32_t numClidren;
    Widget* focussedChild;
};

#endif
#include "gui/widget.h"


Widget::Widget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h,
               uint8_t r, uint8_t g, uint8_t b)
    : KeyboardEventHandler(), parent(parent), x(x), y(y), w(w), h(h), r(r),
      g(g), b(b), Focussable(true) {}

// 活跃窗口，向上传递调用，直到桌面context
void Widget::GetFocus(Widget* widget) {
    if (parent != 0) parent->GetFocus(widget);
}

// 计算像素点的在屏幕中的绝对位置
void Widget::ModelToScreen(int32_t& x, int32_t& y) {
    if (parent != 0) parent->ModelToScreen(x, y);
    x += this->x;
    y += this->y;
}

// 填充context gc中的背景色
void Widget::Draw(GraphicsContext* gc) {
    int X = 0, Y = 0;
    ModelToScreen(X, Y);
    gc->FillRectangle(X, Y, w, h, r, g, b);
}

void Widget::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    if (Focussable) GetFocus(this);
}

void Widget::OnMouseUp(int32_t x, int32_t y, uint8_t button) {}

void Widget::OnMouseMove(int32_t x, int32_t y, int32_t nx, int32_t ny) {}

bool Widget::ContainsCoordinate(int32_t x, int32_t y) {
    return this->x <= x && this->y <= y && this->x + this->w > x
           && this->y + this->h > y;
}


CompositeWidget::CompositeWidget(Widget* parent, int32_t x, int32_t y,
                                 int32_t w, int32_t h, uint8_t r, uint8_t g,
                                 uint8_t b)
    : Widget(parent, x, y, w, h, r, g, b), numClidren(0), focussedChild(0) {}

void CompositeWidget::GetFocus(Widget* widget) {
    focussedChild = widget;
    // TODO: Widget::GetFocus(this);
    if (parent != 0) parent->GetFocus(this);
}

void CompositeWidget::OnKeyDown(char x) {
    if (focussedChild != 0) {
        focussedChild->OnKeyDown(x);
    }
}

void CompositeWidget::OnKeyUp(char x) {
    if (focussedChild != 0) {
        focussedChild->OnKeyUp(x);
    }
}

bool CompositeWidget::AddChild(Widget* child) {
    if (numClidren >= 100) {
        return false;
    }
    children[numClidren++] = child;
    return true;
}

void CompositeWidget::Draw(GraphicsContext* gc) {
    Widget::Draw(gc);  // backgroud
    for (int32_t i = numClidren - 1; ~i; i--) {  // children
        children[i]->Draw(gc);
    }
}

// 向对应的widget传递mouse event
void CompositeWidget::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    for (int32_t i = 0; i < numClidren; i++) {
        if (children[i]->ContainsCoordinate(x - this->x, y - this->y)) {
            children[i]->OnMouseDown(x - this->x, y - this->y, button);
            break;  // only front widget get the event.
        }
    }
}

void CompositeWidget::OnMouseUp(int32_t x, int32_t y, uint8_t button) {
    for (int32_t i = 0; i < numClidren; i++) {
        if (children[i]->ContainsCoordinate(x - this->x, y - this->y)) {
            children[i]->OnMouseUp(x - this->x, y - this->y, button);
            break;
        }
    }
}

void CompositeWidget::OnMouseMove(int32_t x, int32_t y, int32_t nx, int32_t ny) {
    int32_t firstChild = -1;
    for (int32_t i = 0; i < numClidren; i++) {
        if (children[i]->ContainsCoordinate(x - this->x, y - this->y)) {
            // child 中的 相对位置
            children[i]->OnMouseMove(x - this->x, y - this->y, nx - this->x,
                                     ny - this->y);
            firstChild = i;
            break;
        }
    }

    for (int32_t i = 0; i < numClidren; i++) {
        if (children[i]->ContainsCoordinate(nx - this->x, ny - this->y)) {
            // 当前wiget内，不会触发event
            if (firstChild != i) {
                children[i]->OnMouseMove(x - this->x, y - this->y, nx - this->x,
                                         ny - this->y);
            }
            break;
        }
    }
}
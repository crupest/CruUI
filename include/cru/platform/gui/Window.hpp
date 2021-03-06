#pragma once
#include "Base.hpp"

#include "cru/common/Event.hpp"

#include <string_view>

namespace cru::platform::gui {
// Represents a native window, which exposes some low-level events and
// operations.
struct INativeWindow : virtual INativeResource {
  virtual void Close() = 0;

  virtual INativeWindow* GetParent() = 0;

  virtual bool IsVisible() = 0;
  virtual void SetVisible(bool is_visible) = 0;

  virtual Size GetClientSize() = 0;
  virtual void SetClientSize(const Size& size) = 0;

  // Get the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual Rect GetWindowRect() = 0;

  // Set the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual void SetWindowRect(const Rect& rect) = 0;

  // Relative to client lefttop.
  virtual Point GetMousePosition() = 0;

  virtual bool CaptureMouse() = 0;
  virtual bool ReleaseMouse() = 0;

  virtual void SetCursor(std::shared_ptr<ICursor> cursor) = 0;

  virtual void RequestRepaint() = 0;

  // Remember to call EndDraw on return value and destroy it.
  virtual std::unique_ptr<graphics::IPainter> BeginPaint() = 0;

  // Don't use this instance after receive this event.
  virtual IEvent<std::nullptr_t>* DestroyEvent() = 0;
  virtual IEvent<std::nullptr_t>* PaintEvent() = 0;
  virtual IEvent<Size>* ResizeEvent() = 0;
  virtual IEvent<FocusChangeType>* FocusEvent() = 0;
  virtual IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() = 0;
  virtual IEvent<Point>* MouseMoveEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseDownEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseUpEvent() = 0;
  virtual IEvent<NativeMouseWheelEventArgs>* MouseWheelEvent() = 0;
  virtual IEvent<NativeKeyEventArgs>* KeyDownEvent() = 0;
  virtual IEvent<NativeKeyEventArgs>* KeyUpEvent() = 0;

  virtual IInputMethodContext* GetInputMethodContext() = 0;
};
}  // namespace cru::platform::gui

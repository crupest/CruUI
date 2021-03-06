#pragma once
#include "../Base.hpp"

#include "../render/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/platform/gui/Cursor.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/platform/gui/Window.hpp"

#include <functional>
#include <memory>
#include <optional>

namespace cru::ui::host {
class LayoutPaintCycler;

struct AfterLayoutEventArgs {};

struct CreateWindowParams {
  CreateWindowParams(platform::gui::INativeWindow* parent = nullptr,
                     platform::gui::CreateWindowFlag flag = {})
      : parent(parent), flag(flag) {}

  platform::gui::INativeWindow* parent;
  platform::gui::CreateWindowFlag flag;
};

// The bridge between control tree and native window.
class WindowHost : public Object {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::ui::host::WindowHost")

 public:
  WindowHost(controls::Control* root_control);

  CRU_DELETE_COPY(WindowHost)
  CRU_DELETE_MOVE(WindowHost)

  ~WindowHost() override;

 public:
  platform::gui::INativeWindow* GetNativeWindow() { return native_window_; }

  // Do nothing if native window is already created.
  gsl::not_null<platform::gui::INativeWindow*> CreateNativeWindow(
      CreateWindowParams create_window_params = {});

  // Mark the layout as invalid, and arrange a re-layout later.
  // This method could be called more than one times in a message cycle. But
  // layout only takes place once.
  void InvalidateLayout();

  // Mark the paint as invalid, and arrange a re-paint later.
  // This method could be called more than one times in a message cycle. But
  // paint only takes place once.
  void InvalidatePaint();

  IEvent<AfterLayoutEventArgs>* AfterLayoutEvent() {
    return &after_layout_event_;
  }

  void Relayout();
  void Relayout(const Size& available_size);

  void Repaint();

  // Is layout is invalid, wait for relayout and then run the action. Otherwist
  // run it right now.
  void RunAfterLayoutStable(std::function<void()> action);

  // If true, preferred size of root render object is set to window size when
  // measure. Default is true.
  bool IsLayoutPreferToFillWindow() const;
  void SetLayoutPreferToFillWindow(bool value);

  // Get current control that mouse hovers on. This ignores the mouse-capture
  // control. Even when mouse is captured by another control, this function
  // return the control under cursor. You can use `GetMouseCaptureControl` to
  // get more info.
  controls::Control* GetMouseHoverControl() const {
    return mouse_hover_control_;
  }

  //*************** region: focus ***************

  controls::Control* GetFocusControl();

  void SetFocusControl(controls::Control* control);

  //*************** region: focus ***************

  // Pass nullptr to release capture. If mouse is already capture by a control,
  // this capture will fail and return false. If control is identical to the
  // capturing control, capture is not changed and this function will return
  // true.
  //
  // When capturing control changes,
  // appropriate event will be sent. If mouse is not on the capturing control
  // and capture is released, mouse enter event will be sent to the mouse-hover
  // control. If mouse is not on the capturing control and capture is set, mouse
  // leave event will be sent to the mouse-hover control.
  bool CaptureMouseFor(controls::Control* control);

  // Return null if not captured.
  controls::Control* GetMouseCaptureControl();

  controls::Control* HitTest(const Point& point);

  void UpdateCursor();

  IEvent<platform::gui::INativeWindow*>* NativeWindowChangeEvent() {
    return &native_window_change_event_;
  }

  // If window exist, return window actual size. Otherwise if saved rect exists,
  // return it. Otherwise return 0.
  Rect GetWindowRect();

  void SetSavedWindowRect(std::optional<Rect> rect);

  void SetWindowRect(const Rect& rect);

  std::shared_ptr<platform::gui::ICursor> GetOverrideCursor();
  void SetOverrideCursor(std::shared_ptr<platform::gui::ICursor> cursor);

 private:
  //*************** region: native messages ***************
  void OnNativeDestroy(platform::gui::INativeWindow* window, std::nullptr_t);
  void OnNativePaint(platform::gui::INativeWindow* window, std::nullptr_t);
  void OnNativeResize(platform::gui::INativeWindow* window, const Size& size);

  void OnNativeFocus(platform::gui::INativeWindow* window,
                     cru::platform::gui::FocusChangeType focus);

  void OnNativeMouseEnterLeave(platform::gui::INativeWindow* window,
                               cru::platform::gui::MouseEnterLeaveType enter);
  void OnNativeMouseMove(platform::gui::INativeWindow* window,
                         const Point& point);
  void OnNativeMouseDown(platform::gui::INativeWindow* window,
                         const platform::gui::NativeMouseButtonEventArgs& args);
  void OnNativeMouseUp(platform::gui::INativeWindow* window,
                       const platform::gui::NativeMouseButtonEventArgs& args);
  void OnNativeMouseWheel(platform::gui::INativeWindow* window,
                          const platform::gui::NativeMouseWheelEventArgs& args);

  void OnNativeKeyDown(platform::gui::INativeWindow* window,
                       const platform::gui::NativeKeyEventArgs& args);
  void OnNativeKeyUp(platform::gui::INativeWindow* window,
                     const platform::gui::NativeKeyEventArgs& args);

  //*************** region: event dispatcher helper ***************

  void DispatchMouseHoverControlChangeEvent(controls::Control* old_control,
                                            controls::Control* new_control,
                                            const Point& point, bool no_leave,
                                            bool no_enter);

 private:
  controls::Control* root_control_ = nullptr;
  render::RenderObject* root_render_object_ = nullptr;

  platform::gui::INativeWindow* native_window_ = nullptr;

  std::unique_ptr<LayoutPaintCycler> layout_paint_cycler_;

  Event<AfterLayoutEventArgs> after_layout_event_;
  std::vector<std::function<void()> > after_layout_stable_action_;

  std::vector<EventRevokerGuard> event_revoker_guards_;

  controls::Control* mouse_hover_control_ = nullptr;

  controls::Control* focus_control_;

  controls::Control* mouse_captured_control_ = nullptr;

  bool layout_prefer_to_fill_window_ = true;

  Event<platform::gui::INativeWindow*> native_window_change_event_;

  std::optional<Rect> saved_rect_;

  std::shared_ptr<platform::gui::ICursor> override_cursor_;
};
}  // namespace cru::ui::host

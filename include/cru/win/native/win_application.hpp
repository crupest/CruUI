#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/native/ui_applicaition.hpp"

#include <memory>

namespace cru::win::native {
class GodWindow;
class TimerManager;
class WindowManager;

class WinApplication : public Object,
                       public virtual platform::native::UiApplication {
 public:
  static WinApplication* GetInstance();

 private:
  static WinApplication* instance;

 private:
  explicit WinApplication(HINSTANCE h_instance);

 public:
  WinApplication(const WinApplication&) = delete;
  WinApplication(WinApplication&&) = delete;
  WinApplication& operator=(const WinApplication&) = delete;
  WinApplication& operator=(WinApplication&&) = delete;
  ~WinApplication() override;

 public:
  int Run() override;
  void Quit(int quit_code) override;

  void AddOnQuitHandler(const std::function<void()>& handler) override;

  void InvokeLater(const std::function<void()>& action) override;
  unsigned long SetTimeout(std::chrono::milliseconds milliseconds,
                           const std::function<void()>& action) override;
  unsigned long SetInterval(std::chrono::milliseconds milliseconds,
                            const std::function<void()>& action) override;
  void CancelTimer(unsigned long id) override;

  std::vector<platform::native::NativeWindow*> GetAllWindow() override;
  platform::native::NativeWindow* CreateWindow(
      platform::native::NativeWindow* parent) override;

  HINSTANCE GetInstanceHandle() const { return h_instance_; }

  GodWindow* GetGodWindow() const { return god_window_.get(); }
  TimerManager* GetTimerManager() const { return timer_manager_.get(); }
  WindowManager* GetWindowManager() const { return window_manager_.get(); }

 private:
  HINSTANCE h_instance_;

  std::shared_ptr<GodWindow> god_window_;
  std::shared_ptr<TimerManager> timer_manager_;
  std::shared_ptr<WindowManager> window_manager_;

  std::vector<std::function<void()>> quit_handlers_;
};
}  // namespace cru::win::native
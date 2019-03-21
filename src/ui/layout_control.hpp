#pragma once
#include "pre.hpp"

#include "control.hpp"

namespace cru::ui {
class LayoutControl : public Control {
 protected:
  LayoutControl() = default;

 public:
  LayoutControl(const LayoutControl& other) = delete;
  LayoutControl(LayoutControl&& other) = delete;
  LayoutControl& operator=(const LayoutControl& other) = delete;
  LayoutControl& operator=(LayoutControl&& other) = delete;
  ~LayoutControl() override;

  const std::vector<Control*>& GetChildren() const override final {
    return children_;
  }

  void AddChild(Control* control, int position);

  void RemoveChild(int position);

 protected:
  virtual void OnAddChild(Control* child, int position);
  virtual void OnRemoveChild(Control* child, int position);

 private:
  std::vector<Control*> children_;
};
}  // namespace cru::ui
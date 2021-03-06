#pragma once
#include "LayoutControl.hpp"

namespace cru::ui::controls {
class FlexLayout : public LayoutControl {
 public:
  static constexpr std::u16string_view control_type = u"FlexLayout";

  static FlexLayout* Create() { return new FlexLayout(); }

 protected:
  FlexLayout();

 public:
  FlexLayout(const FlexLayout& other) = delete;
  FlexLayout(FlexLayout&& other) = delete;
  FlexLayout& operator=(const FlexLayout& other) = delete;
  FlexLayout& operator=(FlexLayout&& other) = delete;
  ~FlexLayout() override;

  std::u16string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

  FlexMainAlignment GetContentMainAlign() const;
  void SetContentMainAlign(FlexMainAlignment value);

  FlexDirection GetFlexDirection() const;
  void SetFlexDirection(FlexDirection direction);

  FlexCrossAlignment GetItemCrossAlign() const;
  void SetItemCrossAlign(FlexCrossAlignment alignment);

  FlexChildLayoutData GetChildLayoutData(Control* control);
  void SetChildLayoutData(Control* control, FlexChildLayoutData data);

 private:
  std::shared_ptr<render::FlexLayoutRenderObject> render_object_;
};
}  // namespace cru::ui::controls

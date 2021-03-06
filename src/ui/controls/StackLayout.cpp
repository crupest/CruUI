#include "cru/ui/controls/StackLayout.hpp"
#include <memory>

#include "cru/ui/render/StackLayoutRenderObject.hpp"

namespace cru::ui::controls {
using render::StackLayoutRenderObject;

StackLayout::StackLayout() {
  render_object_ = std::make_unique<StackLayoutRenderObject>();
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());
}

StackLayout::~StackLayout() = default;

render::RenderObject* StackLayout::GetRenderObject() const {
  return render_object_.get();
}
}  // namespace cru::ui::controls

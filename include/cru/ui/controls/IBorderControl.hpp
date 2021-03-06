#pragma once
#include "../style/ApplyBorderStyleInfo.hpp"
#include "Base.hpp"
#include "cru/common/Base.hpp"

namespace cru::ui::controls {
struct IBorderControl : virtual Interface {
  virtual void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) = 0;
};
}  // namespace cru::ui::controls

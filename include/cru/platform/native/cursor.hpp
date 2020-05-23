#pragma once
#include "../Resource.hpp"
#include "Base.hpp"

#include <memory>

namespace cru::platform::native {
struct ICursor : virtual INativeResource {};

struct ICursorManager : virtual INativeResource {
  virtual std::shared_ptr<ICursor> GetSystemCursor(SystemCursorType type) = 0;

  // TODO: Add method to create cursor.
};
}  // namespace cru::platform::native

#include "cru/win/WinPreConfig.hpp"

#include "cru/platform/HeapDebug.hpp"

#include <crtdbg.h>

namespace cru::platform {
void SetupHeapDebug() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}
}

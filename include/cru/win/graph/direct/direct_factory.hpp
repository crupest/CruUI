#pragma once
#include "../../win_pre_config.hpp"

namespace cru::platform::graph::win::direct {
// Interface provides access to root d2d resources.
struct IDirectFactory {
  virtual ~IDirectFactory() = default;

  virtual ID2D1Factory1* GetD2D1Factory() const = 0;
  virtual ID2D1DeviceContext* GetD2D1DeviceContext() const = 0;
  virtual ID3D11Device* GetD3D11Device() const = 0;
  virtual IDXGIFactory2* GetDxgiFactory() const = 0;
  virtual IDWriteFactory* GetDWriteFactory() const = 0;
  virtual IDWriteFontCollection* GetSystemFontCollection() const = 0;
};
}  // namespace cru::platform::graph::win::direct
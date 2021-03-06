#pragma once
#include "Resource.hpp"

namespace cru::platform::graphics {

struct IPainter : virtual INativeResource {
  virtual Matrix GetTransform() = 0;
  virtual void SetTransform(const Matrix& matrix) = 0;

  virtual void Clear(const Color& color) = 0;

  virtual void DrawLine(const Point& start, const Point& end, IBrush* brush,
                        float width) = 0;
  virtual void StrokeRectangle(const Rect& rectangle, IBrush* brush,
                               float width) = 0;
  virtual void FillRectangle(const Rect& rectangle, IBrush* brush) = 0;

  virtual void StrokeGeometry(IGeometry* geometry, IBrush* brush,
                              float width) = 0;
  virtual void FillGeometry(IGeometry* geometry, IBrush* brush) = 0;

  virtual void DrawText(const Point& offset, ITextLayout* text_layout,
                        IBrush* brush) = 0;

  virtual void PushLayer(const Rect& bounds) = 0;

  virtual void PopLayer() = 0;

  virtual void EndDraw() = 0;
};
}  // namespace cru::platform::graphics

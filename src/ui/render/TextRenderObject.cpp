#include "cru/ui/render/TextRenderObject.hpp"

#include "../Helper.hpp"
#include "cru/common/Logger.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/graphics/TextLayout.hpp"
#include "cru/platform/graphics/util/Painter.hpp"

#include <algorithm>
#include <limits>

namespace cru::ui::render {
TextRenderObject::TextRenderObject(
    std::shared_ptr<platform::graphics::IBrush> brush,
    std::shared_ptr<platform::graphics::IFont> font,
    std::shared_ptr<platform::graphics::IBrush> selection_brush,
    std::shared_ptr<platform::graphics::IBrush> caret_brush) {
  Expects(brush);
  Expects(font);
  Expects(selection_brush);
  Expects(caret_brush);

  SetChildMode(ChildMode::None);

  brush.swap(brush_);
  font.swap(font_);
  selection_brush.swap(selection_brush_);
  caret_brush.swap(caret_brush_);

  const auto graph_factory = GetGraphFactory();
  text_layout_ = graph_factory->CreateTextLayout(font_, u"");
}

TextRenderObject::~TextRenderObject() = default;

std::u16string TextRenderObject::GetText() const {
  return text_layout_->GetText();
}

std::u16string_view TextRenderObject::GetTextView() const {
  return text_layout_->GetTextView();
}

void TextRenderObject::SetText(std::u16string new_text) {
  text_layout_->SetText(std::move(new_text));
  InvalidateLayout();
}

void TextRenderObject::SetBrush(
    std::shared_ptr<platform::graphics::IBrush> new_brush) {
  Expects(new_brush);
  new_brush.swap(brush_);
  InvalidatePaint();
}

std::shared_ptr<platform::graphics::IFont> TextRenderObject::GetFont() const {
  return text_layout_->GetFont();
}

void TextRenderObject::SetFont(
    std::shared_ptr<platform::graphics::IFont> font) {
  Expects(font);
  text_layout_->SetFont(std::move(font));
}

std::vector<Rect> TextRenderObject::TextRangeRect(const TextRange& text_range) {
  return text_layout_->TextRangeRect(text_range);
}

Point TextRenderObject::TextSinglePoint(gsl::index position, bool trailing) {
  return text_layout_->TextSinglePoint(position, trailing);
}

platform::graphics::TextHitTestResult TextRenderObject::TextHitTest(
    const Point& point) {
  return text_layout_->HitTest(point);
}

void TextRenderObject::SetSelectionRange(std::optional<TextRange> new_range) {
  selection_range_ = std::move(new_range);
  InvalidatePaint();
}

void TextRenderObject::SetSelectionBrush(
    std::shared_ptr<platform::graphics::IBrush> new_brush) {
  Expects(new_brush);
  new_brush.swap(selection_brush_);
  if (selection_range_ && selection_range_->count) {
    InvalidatePaint();
  }
}

void TextRenderObject::SetDrawCaret(bool draw_caret) {
  if (draw_caret_ != draw_caret) {
    draw_caret_ = draw_caret;
    InvalidatePaint();
  }
}

void TextRenderObject::SetCaretPosition(gsl::index position) {
  if (position != caret_position_) {
    caret_position_ = position;
    if (draw_caret_) {
      InvalidatePaint();
    }
  }
}

void TextRenderObject::GetCaretBrush(
    std::shared_ptr<platform::graphics::IBrush> brush) {
  Expects(brush);
  brush.swap(caret_brush_);
  if (draw_caret_) {
    InvalidatePaint();
  }
}

void TextRenderObject::SetCaretWidth(const float width) {
  Expects(width >= 0.0f);

  caret_width_ = width;
  if (draw_caret_) {
    InvalidatePaint();
  }
}

Rect TextRenderObject::GetCaretRectInContent() {
  auto caret_pos = this->caret_position_;
  gsl::index text_size = this->GetText().size();
  if (caret_pos < 0) {
    caret_pos = 0;
  } else if (caret_pos > text_size) {
    caret_pos = text_size;
  }

  const auto caret_top_center =
      this->text_layout_->TextSinglePoint(caret_pos, false);

  const auto font_height = this->font_->GetFontSize();
  const auto caret_width = this->caret_width_;

  auto rect = Rect{caret_top_center.x - caret_width / 2.0f, caret_top_center.y,
                   caret_width, font_height};

  return rect;
}

Rect TextRenderObject::GetCaretRect() {
  auto rect = GetCaretRectInContent();
  const auto content_rect = GetContentRect();

  rect.left += content_rect.left;
  rect.top += content_rect.top;

  return rect;
}

void TextRenderObject::SetMeasureIncludingTrailingSpace(bool including) {
  if (is_measure_including_trailing_space_ == including) return;
  is_measure_including_trailing_space_ = including;
  InvalidateLayout();
}

RenderObject* TextRenderObject::HitTest(const Point& point) {
  const auto padding_rect = GetPaddingRect();
  return padding_rect.IsPointInside(point) ? this : nullptr;
}

void TextRenderObject::OnDrawContent(platform::graphics::IPainter* painter) {
  if (this->selection_range_.has_value()) {
    const auto&& rects =
        text_layout_->TextRangeRect(this->selection_range_.value());
    for (const auto& rect : rects)
      painter->FillRectangle(rect, this->GetSelectionBrush().get());
  }

  painter->DrawText(Point{}, text_layout_.get(), brush_.get());

  if (this->draw_caret_ && this->caret_width_ != 0.0f) {
    painter->FillRectangle(GetCaretRectInContent(), this->caret_brush_.get());
  }
}

Size TextRenderObject::OnMeasureContent(const MeasureRequirement& requirement,
                                        const MeasureSize& preferred_size) {
  float measure_width;
  if (preferred_size.width.IsSpecified())
    measure_width = preferred_size.width.GetLengthOrUndefined();
  else
    measure_width = requirement.max.width.GetLengthOrMax();

  text_layout_->SetMaxWidth(measure_width);
  text_layout_->SetMaxHeight(std::numeric_limits<float>::max());

  const auto text_size =
      text_layout_->GetTextBounds(is_measure_including_trailing_space_)
          .GetSize();
  auto result = text_size;

  result.width = std::max(result.width, preferred_size.width.GetLengthOr0());
  result.width = std::max(result.width, requirement.min.width.GetLengthOr0());

  result.height = std::max(result.height, preferred_size.height.GetLengthOr0());
  result.height =
      std::max(result.height, requirement.min.height.GetLengthOr0());

  return result;
}

void TextRenderObject::OnLayoutContent(const Rect& content_rect) {
  CRU_UNUSED(content_rect)
}

void TextRenderObject::OnAfterLayout() {
  const auto&& size = GetContentRect().GetSize();
  text_layout_->SetMaxWidth(size.width);
  text_layout_->SetMaxHeight(size.height);
}

}  // namespace cru::ui::render

#pragma once
#include "NoChildControl.hpp"

#include "TextHostControlService.hpp"

namespace cru::ui::controls {
class TextBlock : public NoChildControl, public virtual ITextHostControl {
 public:
  static constexpr std::u16string_view control_type = u"TextBlock";

  static TextBlock* Create();
  static TextBlock* Create(std::u16string text, bool selectable = false);

 protected:
  TextBlock();

 public:
  TextBlock(const TextBlock& other) = delete;
  TextBlock(TextBlock&& other) = delete;
  TextBlock& operator=(const TextBlock& other) = delete;
  TextBlock& operator=(TextBlock&& other) = delete;
  ~TextBlock() override;

  std::u16string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

  std::u16string GetText() const;
  void SetText(std::u16string text);

  bool IsSelectable() const;
  void SetSelectable(bool value);

  gsl::not_null<render::TextRenderObject*> GetTextRenderObject() override;
  render::ScrollRenderObject* GetScrollRenderObject() override {
    return nullptr;
  }

 private:
  std::unique_ptr<render::TextRenderObject> text_render_object_;

  std::unique_ptr<TextHostControlService> service_;
};
}  // namespace cru::ui::controls

#pragma once
#include "LayoutRenderObject.hpp"

namespace cru::ui::render {
// Measure Logic (v0.1):
// 1. Layout all children with unspecified(infinate) max main axis length. If
// max cross axis length of parent is specified, then it is passed to children.
//
// 2. Add up main axis length of children to get total length.
//
//  2.1. If preferred main axis length of parent is specified, then compare
//  total length with it. If bigger, shrink is performed. If smaller, expand is
//  performed. And preferred main axis length is used as target length.
//
//  2.2. If preferred main axis length of parent is not specified.
//
//    2.2.1. If max main axis length is specified and total length is bigger
//    than max main axis length, shrink is performed and max main axis length is
//    used as target length.
//
//    2.2.2. If min main axis length is specified and total length is smaller
//    than min main axis length, expand is performed and min main axis length is
//    used as target length.
//
//    2.2.3. If any child has a positive expand factor and max main axis length
//    is specified, then expand is performed and max main axis length is used as
//    target length.
//
//    2.2.4. If any child has a positive expand factor and max main axis length
//    is not specified but min main axis length is specified, then expand is
//    performed and min main axis length is used as target length.
//
// 3. If shrink or expand is needed, then
//
//  3.1. Shrink:
//
//    3.1.1. Find out all shrink_factor > 0 children to form a adjusting list.
//
//    3.1.2. Add up all main axis length of children and minus target length to
//    get the total shrink length. Add up all shrink_factor in adjusting list to
//    get total shrink factor.
//
//    3.1.3. Iterate all children in adjusting list:
//
//      3.1.3.1. Calculate its own shrink length as
//              shrink_factor / total_shrink_factor * total_shrink_length .
//      Use current main axis length of child minus shrink length to get new
//      measure length.
//
//      3.1.3.2. If min main axis length of the child is specified and new
//      measure length is less than it, then min main axis length is used as new
//      measure length.
//
//      3.1.3.3. Or if new measure length is less than 0, then it is coerced to
//      0.
//
//      3.1.3.4. Call measure with max main axis length set to new measure
//      length. Cross axis length requirement is the same as step 1.
//
//      3.1.3.5. After measure, if it has min main axis length specified and
//      actual main axis length is equal to it or its actual main axis length is
//      0, then remove it from adjusting list.
//
//    3.1.4. If adjusting list is not empty, go to step 2).
//
//  3.2. Expand:
//    The same as shrink after you exchange related things.
//
// 4. If final total main axis length exceeeds the max main axis length (if
// specified), then report an error. And result main axis length is the coerced
// length. If final total main axis length is smaller than min main axis length
// (if specified), then coerce the length to the min value but not report error
// and just fill the rest space with blank.
//
// 5. Result cross axis length is the max cross axis length of all children. If
// min cross axis length is specified and result length is smaller than it, then
// result length is coerced to it.
//
class FlexLayoutRenderObject : public LayoutRenderObject<FlexChildLayoutData> {
 public:
  FlexLayoutRenderObject() = default;
  FlexLayoutRenderObject(const FlexLayoutRenderObject& other) = delete;
  FlexLayoutRenderObject& operator=(const FlexLayoutRenderObject& other) =
      delete;
  FlexLayoutRenderObject(FlexLayoutRenderObject&& other) = delete;
  FlexLayoutRenderObject& operator=(FlexLayoutRenderObject&& other) = delete;
  ~FlexLayoutRenderObject() override = default;

  FlexDirection GetFlexDirection() const { return direction_; }
  void SetFlexDirection(FlexDirection direction) {
    direction_ = direction;
    InvalidateLayout();
  }

  FlexMainAlignment GetContentMainAlign() const { return content_main_align_; }
  void SetContentMainAlign(FlexMainAlignment align) {
    content_main_align_ = align;
    InvalidateLayout();
  }

  FlexCrossAlignment GetItemCrossAlign() const { return item_cross_align_; }
  void SetItemCrossAlign(FlexCrossAlignment align) {
    item_cross_align_ = align;
    InvalidateLayout();
  }

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  FlexDirection direction_ = FlexDirection::Horizontal;
  FlexMainAlignment content_main_align_ = FlexMainAlignment::Start;
  FlexCrossAlignment item_cross_align_ = FlexCrossAlignment::Center;
};
}  // namespace cru::ui::render

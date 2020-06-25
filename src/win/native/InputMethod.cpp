#include "cru/win/native/InputMethod.hpp"

#include "cru/common/Logger.hpp"
#include "cru/platform/Check.hpp"
#include "cru/win/Exception.hpp"
#include "cru/win/native/Window.hpp"
#include "cru/win/String.hpp"
#include "DpiUtil.hpp"

#include <vector>

namespace cru::platform::native::win {
AutoHIMC::AutoHIMC(HWND hwnd) : hwnd_(hwnd) {
  Expects(hwnd);
  handle_ = ::ImmGetContext(hwnd);
}

AutoHIMC::AutoHIMC(AutoHIMC&& other)
    : hwnd_(other.hwnd_), handle_(other.handle_) {
  other.hwnd_ = nullptr;
  other.handle_ = nullptr;
}

AutoHIMC& AutoHIMC::operator=(AutoHIMC&& other) {
  if (this != &other) {
    Object::operator=(std::move(other));
    this->hwnd_ = other.hwnd_;
    this->handle_ = other.handle_;
    other.hwnd_ = nullptr;
    other.handle_ = nullptr;
  }
  return *this;
}

AutoHIMC::~AutoHIMC() {
  if (handle_) {
    if (!::ImmReleaseContext(hwnd_, handle_))
      log::Warn("AutoHIMC: Failed to release HIMC.");
  }
}

// copied from chromium
namespace {
// Determines whether or not the given attribute represents a target
// (a.k.a. a selection).
bool IsTargetAttribute(char attribute) {
  return (attribute == ATTR_TARGET_CONVERTED ||
          attribute == ATTR_TARGET_NOTCONVERTED);
}
// Helper function for ImeInput::GetCompositionInfo() method, to get the target
// range that's selected by the user in the current composition string.
void GetCompositionTargetRange(HIMC imm_context, int* target_start,
                               int* target_end) {
  int attribute_size =
      ::ImmGetCompositionString(imm_context, GCS_COMPATTR, NULL, 0);
  if (attribute_size > 0) {
    int start = 0;
    int end = 0;
    std::vector<char> attribute_data(attribute_size);
    ::ImmGetCompositionString(imm_context, GCS_COMPATTR, attribute_data.data(),
                              attribute_size);
    for (start = 0; start < attribute_size; ++start) {
      if (IsTargetAttribute(attribute_data[start])) break;
    }
    for (end = start; end < attribute_size; ++end) {
      if (!IsTargetAttribute(attribute_data[end])) break;
    }
    if (start == attribute_size) {
      // This composition clause does not contain any target clauses,
      // i.e. this clauses is an input clause.
      // We treat the whole composition as a target clause.
      start = 0;
      end = attribute_size;
    }
    *target_start = start;
    *target_end = end;
  }
}
// Helper function for ImeInput::GetCompositionInfo() method, to get underlines
// information of the current composition string.
CompositionClauses GetCompositionClauses(HIMC imm_context, int target_start,
                                         int target_end) {
  CompositionClauses result;
  int clause_size =
      ::ImmGetCompositionString(imm_context, GCS_COMPCLAUSE, NULL, 0);
  int clause_length = clause_size / sizeof(std::uint32_t);
  if (clause_length) {
    result.reserve(clause_length - 1);
    std::vector<std::uint32_t> clause_data(clause_length);
    ::ImmGetCompositionString(imm_context, GCS_COMPCLAUSE, clause_data.data(),
                              clause_size);
    for (int i = 0; i < clause_length - 1; ++i) {
      CompositionClause clause;
      clause.start = clause_data[i];
      clause.end = clause_data[i + 1];
      clause.target = false;
      // Use thick underline for the target clause.
      if (clause.start >= target_start && clause.end <= target_end) {
        clause.target = true;
      }
      result.push_back(clause);
    }
  }
  return result;
}

std::wstring GetString(HIMC imm_context) {
  LONG string_size =
      ::ImmGetCompositionString(imm_context, GCS_COMPSTR, NULL, 0);
  std::wstring result((string_size / sizeof(wchar_t)), 0);
  ::ImmGetCompositionString(imm_context, GCS_COMPSTR, result.data(),
                            string_size);
  return result;
}

std::wstring GetResultString(HIMC imm_context) {
  LONG string_size =
      ::ImmGetCompositionString(imm_context, GCS_RESULTSTR, NULL, 0);
  std::wstring result((string_size / sizeof(wchar_t)), 0);
  ::ImmGetCompositionString(imm_context, GCS_RESULTSTR, result.data(),
                            string_size);
  return result;
}

CompositionText GetCompositionInfo(HIMC imm_context) {
  // We only care about GCS_COMPATTR, GCS_COMPCLAUSE and GCS_CURSORPOS, and
  // convert them into underlines and selection range respectively.

  auto w_text = GetString(imm_context);

  int w_length = static_cast<int>(w_text.length());
  // Find out the range selected by the user.
  int w_target_start = w_length;
  int w_target_end = w_length;
  GetCompositionTargetRange(imm_context, &w_target_start, &w_target_end);

  auto clauses =
      GetCompositionClauses(imm_context, w_target_start, w_target_end);

  int w_cursor = ::ImmGetCompositionString(imm_context, GCS_CURSORPOS, NULL, 0);

  auto text = platform::win::ToUtf8String(w_text);
  for (auto& clause : clauses) {
    clause.start = platform::win::IndexUtf16ToUtf8(w_text, clause.start, text);
    clause.end = platform::win::IndexUtf16ToUtf8(w_text, clause.end, text);
  }
  int cursor = platform::win::IndexUtf16ToUtf8(w_text, w_cursor, text);

  return CompositionText{std::move(text), std::move(clauses),
                         TextRange{cursor}};
}

}  // namespace

WinInputMethodContext::WinInputMethodContext(
    gsl::not_null<WinNativeWindow*> window)
    : native_window_resolver_(window->GetResolver()) {
  event_revoker_guards_.push_back(
      EventRevokerGuard(window->NativeMessageEvent()->AddHandler(
          std::bind(&WinInputMethodContext::OnWindowNativeMessage, this,
                    std::placeholders::_1))));
}

WinInputMethodContext::~WinInputMethodContext() {}

void WinInputMethodContext::EnableIME() {
  const auto native_window = Resolve(native_window_resolver_.get());
  if (native_window == nullptr) return;
  const auto hwnd = native_window->GetWindowHandle();

  if (::ImmAssociateContextEx(hwnd, nullptr, IACE_DEFAULT) == FALSE) {
    log::Warn("WinInputMethodContext: Failed to enable ime.");
  }
}

void WinInputMethodContext::DisableIME() {
  const auto native_window = Resolve(native_window_resolver_.get());
  if (native_window == nullptr) return;
  const auto hwnd = native_window->GetWindowHandle();

  AutoHIMC himc{hwnd};

  if (!::ImmNotifyIME(himc.Get(), NI_COMPOSITIONSTR, CPS_COMPLETE, 0)) {
    log::Warn(
        "WinInputMethodContext: Failed to complete composition before disable "
        "ime.");
  }

  if (::ImmAssociateContextEx(hwnd, nullptr, 0) == FALSE) {
    log::Warn("WinInputMethodContext: Failed to disable ime.");
  }
}

void WinInputMethodContext::CompleteComposition() {
  auto optional_himc = TryGetHIMC();
  if (!optional_himc.has_value()) return;
  auto himc = *std::move(optional_himc);

  if (!::ImmNotifyIME(himc.Get(), NI_COMPOSITIONSTR, CPS_COMPLETE, 0)) {
    log::Warn("WinInputMethodContext: Failed to complete composition.");
  }
}

void WinInputMethodContext::CancelComposition() {
  auto optional_himc = TryGetHIMC();
  if (!optional_himc.has_value()) return;
  auto himc = *std::move(optional_himc);

  if (!::ImmNotifyIME(himc.Get(), NI_COMPOSITIONSTR, CPS_CANCEL, 0)) {
    log::Warn("WinInputMethodContext: Failed to complete composition.");
  }
}

CompositionText WinInputMethodContext::GetCompositionText() {
  auto optional_himc = TryGetHIMC();
  if (!optional_himc.has_value()) return CompositionText{};
  auto himc = *std::move(optional_himc);

  return GetCompositionInfo(himc.Get());
}

void WinInputMethodContext::SetCandidateWindowPosition(const Point& point) {
  auto optional_himc = TryGetHIMC();
  if (!optional_himc.has_value()) return;
  auto himc = *std::move(optional_himc);

  ::CANDIDATEFORM form;
  form.dwIndex = 1;
  form.dwStyle = CFS_CANDIDATEPOS;
  form.ptCurrentPos = DipToPi(point);

  if (!::ImmSetCandidateWindow(himc.Get(), &form))
    log::Debug(
        "WinInputMethodContext: Failed to set input method candidate window "
        "position.");
}

IEvent<std::nullptr_t>* WinInputMethodContext::CompositionStartEvent() {
  return &composition_start_event_;
}

IEvent<std::nullptr_t>* WinInputMethodContext::CompositionEndEvent() {
  return &composition_end_event_;
};

IEvent<std::nullptr_t>* WinInputMethodContext::CompositionEvent() {
  return &composition_event_;
}

IEvent<std::string_view>* WinInputMethodContext::TextEvent() {
  return &text_event_;
}

void WinInputMethodContext::OnWindowNativeMessage(
    WindowNativeMessageEventArgs& args) {
  const auto& message = args.GetWindowMessage();
  switch (message.msg) {
    case WM_CHAR: {
      const auto c = static_cast<wchar_t>(message.w_param);
      if (platform::win::IsSurrogatePair(c)) {
        // I don't think this will happen because normal key strike without ime
        // should only trigger ascci character. If it is a charater from
        // supplementary planes, it should be handled with ime messages.
        log::Warn(
            "WinInputMethodContext: A WM_CHAR message for character from "
            "supplementary planes is ignored.");
      } else {
        wchar_t s[1] = {c};
        auto str = platform::win::ToUtf8String({s, 1});
        text_event_.Raise(str);
      }
      args.HandleWithResult(0);
      break;
    }
    case WM_IME_COMPOSITION: {
      composition_event_.Raise(nullptr);
      auto composition_text = GetCompositionText();
      log::Debug(
          "WinInputMethodContext: WM_IME_COMPOSITION composition text:\n{}",
          composition_text);
      if (message.l_param & GCS_RESULTSTR) {
        auto result_string = GetResultString();
        text_event_.Raise(result_string);
      }
      break;
    }
    case WM_IME_STARTCOMPOSITION: {
      composition_start_event_.Raise(nullptr);
      break;
    }
    case WM_IME_ENDCOMPOSITION: {
      composition_end_event_.Raise(nullptr);
      break;
    }
  }
}

std::string WinInputMethodContext::GetResultString() {
  auto optional_himc = TryGetHIMC();
  if (!optional_himc.has_value()) return "";
  auto himc = *std::move(optional_himc);

  auto w_result = win::GetResultString(himc.Get());
  return platform::win::ToUtf8String(w_result);
}

std::optional<AutoHIMC> WinInputMethodContext::TryGetHIMC() {
  const auto native_window = Resolve(native_window_resolver_.get());
  if (native_window == nullptr) return std::nullopt;
  const auto hwnd = native_window->GetWindowHandle();
  return AutoHIMC{hwnd};
}

WinInputMethodManager::WinInputMethodManager(WinUiApplication*) {}

WinInputMethodManager::~WinInputMethodManager() {}

std::unique_ptr<IInputMethodContext> WinInputMethodManager::GetContext(
    INativeWindow* window) {
  Expects(window);
  const auto w = CheckPlatform<WinNativeWindow>(window, GetPlatformId());
  return std::make_unique<WinInputMethodContext>(w);
}
}  // namespace cru::platform::native::win
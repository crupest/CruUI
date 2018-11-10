#include "application.hpp"
#include "ui/window.hpp"
#include "ui/controls/linear_layout.hpp"
#include "ui/controls/text_block.hpp"
#include "ui/controls/toggle_button.hpp"
#include "ui/controls/button.hpp"
#include "ui/controls/text_box.hpp"
#include "ui/controls/list_item.hpp"

using cru::String;
using cru::StringView;
using cru::Application;
using cru::ui::Rect;
using cru::ui::Window;
using cru::ui::Alignment;
using cru::ui::LayoutSideParams;
using cru::ui::Thickness;
using cru::ui::ControlList;
using cru::ui::CreateWithLayout;
using cru::ui::controls::LinearLayout;
using cru::ui::controls::TextBlock;
using cru::ui::controls::ToggleButton;
using cru::ui::controls::Button;
using cru::ui::controls::TextBox;
using cru::ui::controls::ListItem;

int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow) {

    Application application(hInstance);

    const auto window = Window::CreateOverlapped();
    /*
    window.native_message_event.AddHandler([](cru::ui::events::WindowNativeMessageEventArgs& args)
    {
        if (args.GetWindowMessage().msg == WM_PAINT)
        {
            OutputDebugStringW(L"Paint!\n");
            //args.SetResult(0);
        }
    });
    */
    /*
    // test1
    cru::ui::controls::TextBlock text_block;
    text_block.SetText(L"Hello world!");
    text_block.SetSize(cru::ui::Size(200, 30));
    window.AddChild(&text_block);

    std::array<D2D_COLOR_F, 4> colors =
    {
        D2D1::ColorF(D2D1::ColorF::Blue),
        D2D1::ColorF(D2D1::ColorF::Yellow),
        D2D1::ColorF(D2D1::ColorF::Green),
        D2D1::ColorF(D2D1::ColorF::Red)
    };

    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<decltype(colors.size())> uni(0, colors.size() - 1); // guaranteed unbiased


    window.draw_event.AddHandler([&](cru::ui::events::DrawEventArgs& args) {
        auto device_context = args.GetDeviceContext();

        ID2D1SolidColorBrush* brush;
        device_context->CreateSolidColorBrush(colors[uni(rng)], &brush);

        device_context->FillRectangle(D2D1::RectF(100.0f, 100.0f, 300.0f, 200.0f), brush);

        brush->Release();
    });

    cru::SetTimeout(2.0, [&window]() {
        window.Repaint();

        auto task = cru::SetInterval(0.5, [&window]() {
            window.Repaint();
        });

        cru::SetTimeout(4, [task]() {
            task->Cancel();
            task->Cancel(); // test for idempotency.
        });
    });
    */

    
    //test 2
    const auto layout = CreateWithLayout<LinearLayout>(LayoutSideParams::Exactly(500), LayoutSideParams::Content());

    layout->mouse_click_event.AddHandler([layout](cru::ui::events::MouseButtonEventArgs& args)
    {
        if (args.GetSender() == args.GetOriginalSender())
            layout->AddChild(TextBlock::Create(L"Layout is clicked!"));
    });

    {
        const auto inner_layout = CreateWithLayout<LinearLayout>(LayoutSideParams::Content(Alignment::End), LayoutSideParams::Content(), LinearLayout::Orientation::Horizontal);

        inner_layout->AddChild(TextBlock::Create(L"Toggle debug border"));

        const auto toggle_button = ToggleButton::Create();
#ifdef CRU_DEBUG_LAYOUT
        toggle_button->toggle_event.AddHandler([&window](cru::ui::events::ToggleEventArgs& args)
        {
            window->SetDebugLayout(args.GetNewState());
        });
#endif
        inner_layout->AddChild(toggle_button);
        layout->AddChild(inner_layout);
    }

    {
        const auto button = Button::Create();
        button->GetLayoutParams()->padding = Thickness(20, 5);
        button->AddChild(TextBlock::Create(L"Show popup window parenting this."));
        button->mouse_click_event.AddHandler([window](auto)
        {
            const auto popup = Window::CreatePopup(window);

            auto create_menu_item = [](const String& text) -> ListItem*
            {
                return CreateWithLayout<ListItem>(
                    LayoutSideParams::Content(Alignment::Start),
                    LayoutSideParams::Content(Alignment::Start),
                    ControlList{ TextBlock::Create(text) }
                );
            };

            const auto menu = LinearLayout::Create(LinearLayout::Orientation::Vertical, ControlList{
                create_menu_item(L"copy"),
                create_menu_item(L"cut"),
                create_menu_item(L"paste")
            });

            popup->AddChild(menu);

            popup->SetSizeFitContent();

            popup->Show();
        });
        layout->AddChild(button);
    }

    {
        const auto button = Button::Create();
        button->GetLayoutParams()->padding = Thickness(20, 5);
        button->AddChild(TextBlock::Create(L"Show popup window parenting null."));
        button->mouse_click_event.AddHandler([](auto)
        {
            auto popup = Window::CreatePopup(nullptr);
            popup->SetWindowRect(Rect(100, 100, 300, 300));
            popup->Show();
        });
        layout->AddChild(button);
    }
    
    {
        const auto button = Button::Create();
        button->GetLayoutParams()->padding = Thickness(20, 5);
        button->AddChild(TextBlock::Create(L"Show popup window with caption."));
        button->mouse_click_event.AddHandler([](auto)
        {
            auto popup = Window::CreatePopup(nullptr, true);
            popup->SetWindowRect(Rect(100, 100, 300, 300));
            popup->Show();
        });
        layout->AddChild(button);
    }

    {
        const auto text_block = CreateWithLayout<TextBlock>(LayoutSideParams::Exactly(200), LayoutSideParams::Exactly(80), L"Hello World!!!");

        text_block->mouse_click_event.AddHandler([layout](cru::ui::events::MouseButtonEventArgs& args)
        {
            layout->AddChild(TextBlock::Create(L"Hello world is clicked!"));
        });

        layout->AddChild(text_block);
    }

    {
        const auto text_box = TextBox::Create();
        text_box->GetLayoutParams()->width.min = 50.0f;
        text_box->GetLayoutParams()->width.max = 100.0f;
        layout->AddChild(text_box);
    }

    {
        const auto text_block = CreateWithLayout<TextBlock>(LayoutSideParams::Stretch(), LayoutSideParams::Stretch(), L"This is a very very very very very long sentence!!!");
        text_block->SetSelectable(true);
        layout->AddChild(text_block);
    }

    layout->AddChild(CreateWithLayout<TextBlock>(LayoutSideParams::Content(Alignment::Start), LayoutSideParams::Content(), L"This is a little short sentence!!!"));
    layout->AddChild(CreateWithLayout<TextBlock>(LayoutSideParams::Content(Alignment::End), LayoutSideParams::Stretch(), L"By crupest!!!"));


    window->AddChild(layout);

    /*
    window.AddChild(
        CreateWithLayout<Border>(LayoutSideParams::Exactly(200), LayoutSideParams::Content(),
            std::initializer_list<cru::ui::Control*>{
                CreateWithLayout<TextBox>(LayoutSideParams::Stretch(), LayoutSideParams::Content())
            }
    ));
    */

    /* test 3
    const auto linear_layout = CreateWithLayout<LinearLayout>(Thickness(50, 50), Thickness(50, 50), LinearLayout::Orientation::Vertical, ControlList{
            Button::Create({
                TextBlock::Create(L"Button")
            }),
            CreateWithLayout<TextBox>(Thickness(30), Thickness(20))
        });

    linear_layout->SetBordered(true);

    window.AddChild(linear_layout);
    */


    window->Show();

    return application.Run();
}

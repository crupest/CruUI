#include "application.h"
#include "ui/window.h"
#include "ui/controls/linear_layout.h"
#include "ui/controls/text_block.h"
#include "ui/controls/toggle_button.h"


using cru::String;
using cru::Application;
using cru::ui::Window;
using cru::ui::MeasureMode;
using cru::ui::Alignment;
using cru::ui::LayoutLength;
using cru::ui::controls::LinearLayout;
using cru::ui::controls::TextBlock;
using cru::ui::controls::ToggleButton;


int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow) {

    Application application(hInstance);
    Window window;

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

    const auto layout = LinearLayout::Create();

    layout->GetLayoutParams()->width = LayoutLength::Exactly(500);

    layout->mouse_click_event.AddHandler([layout](cru::ui::events::MouseButtonEventArgs& args)
    {
        if (args.GetSender() == args.GetOriginalSender())
            layout->AddChild(TextBlock::Create(L"Layout is clicked!"));
    });

    const auto inner_layout = LinearLayout::Create(LinearLayout::Orientation::Horizontal);
    inner_layout->GetLayoutParams()->width.alignment = Alignment::End;

    layout->AddChild(inner_layout);

    inner_layout->AddChild(TextBlock::Create(L"Toggle debug border"));

    const auto toggle_button = ToggleButton::Create();
    toggle_button->toggle_event.AddHandler([&window](cru::ui::events::ToggleEventArgs& args)
    {
        window.SetDebugDrawControlBorder(args.GetNewState());
    });

    inner_layout->AddChild(toggle_button);

    auto&& create_text_block = [](const String& text, const LayoutLength& width = LayoutLength::Content(), const LayoutLength& height = LayoutLength::Content())
    {
        const auto text_block = TextBlock::Create(text);
        text_block->GetLayoutParams()->width = width;
        text_block->GetLayoutParams()->height = height;
        return text_block;
    };

    {
        const auto text_block = create_text_block(L"Hello World!!!", LayoutLength::Exactly(200), LayoutLength::Exactly(80));

        text_block->mouse_click_event.AddHandler([layout](cru::ui::events::MouseButtonEventArgs& args)
        {
            layout->AddChild(TextBlock::Create(L"Hello world is clicked!"));
        });

        layout->AddChild(text_block);
    }

    {
        const auto text_block = create_text_block(L"This is a very very very very very long sentence!!!", LayoutLength::Stretch(), LayoutLength::Stretch());
        text_block->SetSelectable(true);
        layout->AddChild(text_block);
    }

    {
        const auto text_block = TextBlock::Create(L"This is a little short sentence!!!");
        text_block->GetLayoutParams()->width.alignment = Alignment::Start;
        layout->AddChild(text_block);
    }

    layout->AddChild(create_text_block(L"By crupest!!!", LayoutLength::Stretch(), LayoutLength::Stretch()));


    window.AddChild(layout);

    window.Show();

    return application.Run();
}

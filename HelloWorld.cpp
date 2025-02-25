// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <optional>
#include <variant>

namespace DeclarativeUI {

template <typename T>
concept CreateAndAddable = requires(T widget, wxWindow* window, wxSizer* sizer) {
    widget.createAndAdd(window, sizer, wxSizerFlags {});
};

}

namespace DeclarativeUI::details {

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <typename W>
struct Widget {
    explicit Widget(wxWindowID id = wxID_ANY, std::string str = std::string {}, std::optional<wxSizerFlags> flags = {})
        : id(id)
        , str(std::move(str))
        , flags(flags)
    {
    }

    Widget(wxWindowID id, std::optional<wxSizerFlags> flags)
        : id(id)
        , flags(flags)
    {
    }

    explicit Widget(std::string str, std::optional<wxSizerFlags> flags = {})
        : id(wxID_ANY)
        , str(std::move(str))
        , flags(flags)
    {
    }

    explicit Widget(std::optional<wxSizerFlags> flags)
        : id(wxID_ANY)
        , flags(flags)
    {
    }

    auto createAndAdd(wxWindow* parent, wxSizer* sizer, wxSizerFlags parentFlags)
    {
        sizer->Add(
            bindHandler(new W(parent, id, str, position, size)),
            flags ? *flags : parentFlags);
    }

    auto withSize(wxSize size_) -> Widget<W>&
    {
        size = size_;
        return *this;
    }

    auto withWidth(int size_) -> Widget<W>&
    {
        size.SetWidth(size_);
        return *this;
    }

    auto withHeight(int size_) -> Widget<W>&
    {
        size.SetHeight(size_);
        return *this;
    }

    using Handler = std::variant<std::function<void(wxCommandEvent&)>, std::function<void()>>;
    auto bind(Handler handler) -> Widget<W>&
    {
        boundedHandler = handler;
        return *this;
    }

private:
    wxWindowID id;
    wxPoint position = wxDefaultPosition;
    wxSize size = wxDefaultSize;
    std::string str;
    std::optional<wxSizerFlags> flags;
    std::optional<Handler> boundedHandler;

    auto bindHandler(wxWindow* widget) -> wxWindow*
    {
        if (boundedHandler) {
            std::visit(
                details::overloaded {
                    [&widget](std::function<void(wxCommandEvent&)> func) {
                        widget->Bind(wxEVT_BUTTON, func);
                    },
                    [&widget](std::function<void()> func) {
                        widget->Bind(wxEVT_BUTTON, [func](wxCommandEvent&) {
                            func();
                        });
                    },
                },
                *boundedHandler);
        }
        return widget;
    }
};

template <CreateAndAddable... W>
struct Sizer {
    Sizer(wxOrientation orientation, wxSizerFlags flags, W... widgets)
        : orientation(orientation)
        , flags(flags)
        , widgets(std::make_tuple(widgets...))
    {
    }

    Sizer(wxOrientation orientation, W... widgets)
        : orientation(orientation)
        , widgets(std::make_tuple(widgets...))
    {
    }

    auto createAndAdd(wxWindow* parent, wxSizer* parentSizer, wxSizerFlags parentFlags)
    {
        auto* sizer = new wxBoxSizer(orientation);
        std::apply([this, parent, sizer, parentFlags](auto&&... tupleArg) {
            (tupleArg.createAndAdd(parent, sizer, flags.value_or(parentFlags)), ...);
        },
            widgets);
        parentSizer->Add(sizer, parentFlags);
        return sizer;
    }

    auto attachTo(wxWindow* parent)
    {
        auto* sizer = new wxBoxSizer(orientation);
        std::apply([parent, sizer, flags = flags ? *flags : wxSizerFlags()](auto&&... tupleArg) {
            (tupleArg.createAndAdd(parent, sizer, flags), ...);
        },
            widgets);
        parent->SetSizerAndFit(sizer);
        return sizer;
    }

private:
    wxOrientation orientation;
    std::optional<wxSizerFlags> flags;
    std::tuple<W...> widgets;
};

}

namespace DeclarativeUI {

template <CreateAndAddable... W>
struct HSizer : details::Sizer<W...> {
    using super = details::Sizer<W...>;
    HSizer(W... widgets)
        : super(wxHORIZONTAL, widgets...)
    {
    }
    HSizer(wxSizerFlags flags, W... widgets)
        : super(wxHORIZONTAL, flags, widgets...)
    {
    }
};

template <CreateAndAddable... W>
struct VSizer : details::Sizer<W...> {
    using super = details::Sizer<W...>;
    VSizer(W... widgets)
        : super(wxVERTICAL, widgets...)
    {
    }
    VSizer(wxSizerFlags flags, W... widgets)
        : super(wxVERTICAL, flags, widgets...)
    {
    }
};

using TextCtrl = details::Widget<wxTextCtrl>;
using Button = details::Widget<wxButton>;
using Text = details::Widget<wxStaticText>;

#define UNITTEST(WIDGET)                               \
    do {                                               \
        WIDGET {};                                     \
        WIDGET { wxID_ANY };                           \
        WIDGET { "Hello" };                            \
        WIDGET { wxID_ANY, "Hello" };                  \
        WIDGET { wxSizerFlags {} };                    \
        WIDGET { wxID_ANY, wxSizerFlags {} };          \
        WIDGET { "Hello", wxSizerFlags {} };           \
        WIDGET { wxID_ANY, "Hello", wxSizerFlags {} }; \
    } while (0)

void UnitTest()
{
    UNITTEST(TextCtrl);
    UNITTEST(Button);
    UNITTEST(Text);
}

}

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
};

enum { ID_Hello = 1 };

// clang-format off
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Hello, MyFrame::OnHello)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_BUTTON(wxID_EXIT, MyFrame::OnExit)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);
// clang-format on

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame("Hello World", wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    using namespace DeclarativeUI;
    // Create and layout the controls.
    VSizer {
        wxSizerFlags().Expand().Border(),
        HSizer {
            TextCtrl { wxSizerFlags(1).Expand().Border() }
                .withWidth(64),
            Button { "Right" }.bind([] {
                wxLogMessage("Hello world from wxWidgets!");
            }),
        },
        HSizer {
            Button { "Left" },
            Text { "Cat" },
        },
        Button { wxID_EXIT, "Exit" },
    }
        .attachTo(this);
}

void MyFrame::OnExit([[maybe_unused]] wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout([[maybe_unused]] wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets' Hello world sample", "About Hello World",
        wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello([[maybe_unused]] wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

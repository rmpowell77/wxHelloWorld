// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <optional>

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

namespace DeclarativeUI {

template <typename T>
concept CreateAndAddable = requires(T widget, wxWindow* window, wxSizer* sizer) {
    widget.createAndAdd(window, sizer, wxSizerFlags {});
};

template <typename W>
struct Widget {
    explicit Widget(wxWindowID id = wxID_ANY, std::string str = {})
        : id_(id)
        , str_(std::move(str))
    {
    }

    explicit Widget(std::string str)
        : str_(std::move(str))
    {
    }

    auto createAndAdd(wxWindow* parent, wxSizer* sizer, wxSizerFlags suppliedFlags)
    {
        sizer->Add(new W(parent, id_, str_, pos_, size_, style_), flags_.value_or(suppliedFlags));
    }

    auto withFlags(wxSizerFlags flags) -> Widget<W>&
    {
        flags_ = flags;
        return *this;
    }

    auto withPos(wxPoint pos) -> Widget<W>&
    {
        pos_ = pos;
        return *this;
    }

    auto withSize(wxSize size) -> Widget<W>&
    {
        size_ = size;
        return *this;
    }

    auto withWidth(int width) -> Widget<W>&
    {
        size_.SetWidth(width);
        return *this;
    }

    auto withHeight(int height) -> Widget<W>&
    {
        size_.SetHeight(height);
        return *this;
    }

    auto withStyle(long style) -> Widget<W>&
    {
        style_ = style;
        return *this;
    }

private:
    wxWindowID id_ { wxID_ANY };
    std::string str_;
    wxPoint pos_ { wxDefaultPosition };
    wxSize size_ { wxDefaultSize };
    long style_ { 0 };
    std::optional<wxSizerFlags> flags_;
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

    auto createAndAdd(wxWindow* parent, wxSizerFlags parentFlags)
    {
        auto* sizer = new wxBoxSizer(orientation);
        std::apply([this, parent, sizer, parentFlags](auto&&... tupleArg) {
            (tupleArg.createAndAdd(parent, sizer, flags.value_or(parentFlags)), ...);
        },
            widgets);
        return sizer;
    }

    auto createAndAdd(wxWindow* parent, wxSizer* parentSizer, wxSizerFlags parentFlags)
    {
        auto* sizer = createAndAdd(parent, flags.value_or(parentFlags));
        parentSizer->Add(sizer, flags.value_or(parentFlags));
        return sizer;
    }

    auto fitTo(wxWindow* parent)
    {
        auto* sizer = createAndAdd(parent, flags.value_or(wxSizerFlags()));
        parent->SetSizerAndFit(sizer);
        return sizer;
    }

    wxOrientation orientation;
    std::optional<wxSizerFlags> flags;
    std::tuple<W...> widgets;
};

template <CreateAndAddable... W>
struct HSizer : Sizer<W...> {
    HSizer(W... widgets)
        : Sizer<W...>(wxHORIZONTAL, widgets...)
    {
    }
    HSizer(wxSizerFlags flags, W... widgets)
        : Sizer<W...>(wxHORIZONTAL, flags, widgets...)
    {
    }
};

template <CreateAndAddable... W>
struct VSizer : Sizer<W...> {
    VSizer(W... widgets)
        : Sizer<W...>(wxVERTICAL, widgets...)
    {
    }
    VSizer(wxSizerFlags flags, W... widgets)
        : Sizer<W...>(wxVERTICAL, flags, widgets...)
    {
    }
};

using TextCtrl = Widget<wxTextCtrl>;
using Button = Widget<wxButton>;
using Text = Widget<wxStaticText>;

static_assert(CreateAndAddable<TextCtrl>);
static_assert(CreateAndAddable<Button>);
static_assert(CreateAndAddable<Text>);

static_assert(CreateAndAddable<HSizer<Button, TextCtrl>>);
static_assert(CreateAndAddable<HSizer<Text, Button>>);

static_assert(CreateAndAddable<VSizer<HSizer<Button, TextCtrl>, HSizer<Text, Button>>>);

}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    CreateStatusBar(1);
    using namespace DeclarativeUI;
    // Create and layout the controls.
    VSizer {
        wxSizerFlags().Border(),
        HSizer {
            wxSizerFlags().Border().Expand(),
            Button { "Click" },
            TextCtrl { "Dog" }
                .withWidth(100)
                .withFlags(wxSizerFlags(1).Border()) },
        HSizer {
            Text { wxID_ANY, "Cat" },
            Button { wxID_EXIT, "Done" } }
    }.fitTo(this);
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

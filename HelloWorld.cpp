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
    Widget(wxWindowID id, std::string str, std::optional<wxSizerFlags> flags = {})
        : id_(id)
        , str_(std::move(str))
        , flags_(flags)
    {
    }

    auto createAndAdd(wxWindow* parent, wxSizer* sizer, wxSizerFlags suppliedFlags)
    {
        sizer->Add(new W(parent, id_, str_), flags_.value_or(suppliedFlags));
    }

private:
    wxWindowID id_ { wxID_ANY };
    std::string str_;
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

}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    CreateStatusBar(1);
    using namespace DeclarativeUI;
    // Create and layout the controls.
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    HSizer {
        Button { wxID_ANY, "Click" },
        TextCtrl { wxID_ANY, "Dog", wxSizerFlags(1).Border() }
    }.createAndAdd(this, sizer, wxSizerFlags().Border().Expand());

    HSizer {
        Text { wxID_ANY, "Cat" },
        Button { wxID_EXIT, "Done" }
    }.createAndAdd(this, sizer, wxSizerFlags().Border());

    SetSizerAndFit(sizer);
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

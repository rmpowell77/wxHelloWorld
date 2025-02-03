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
auto createAndAdd(wxWindow* parent, wxSizer* sizer, wxSizerFlags flags, std::tuple<W...> widgets)
{
    std::apply([parent, sizer, flags](auto&&... tupleArg) {
        (tupleArg.createAndAdd(parent, sizer, flags), ...);
    },
        widgets);
}

template <CreateAndAddable... W>
struct Sizer {
    Sizer(wxOrientation orientation, wxSizerFlags flags, W... widgets)
        : orientation(orientation)
        , flags(flags)
        , widgets(std::make_tuple(widgets...))
    {
    }

    auto createAndAdd(wxWindow* parent, wxSizer* parentSizer, wxSizerFlags parentFlags)
    {
        auto* sizer = new wxBoxSizer(orientation);
        ::createAndAdd(parent, sizer, flags, widgets);
        parentSizer->Add(sizer, parentFlags);
        return sizer;
    }

    wxOrientation orientation;
    wxSizerFlags flags;
    std::tuple<W...> widgets;
};

using TextCtrl = Widget<wxTextCtrl>;
using Button = Widget<wxButton>;
using Text = Widget<wxStaticText>;

static_assert(CreateAndAddable<TextCtrl>);
static_assert(CreateAndAddable<Button>);
static_assert(CreateAndAddable<Text>);

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    auto* topSizer = new wxBoxSizer(wxVERTICAL);

    Sizer {
        wxHORIZONTAL,
        wxSizerFlags().Border(),
        Button { wxID_ANY, "Click" },
        TextCtrl { wxID_ANY, "Dog", wxSizerFlags(1).Border() }
    }.createAndAdd(this, topSizer, wxSizerFlags().Border().Expand());

    Sizer {
        wxHORIZONTAL,
        wxSizerFlags().Border(),
        Text { wxID_ANY, "Cat" },
        Button { wxID_EXIT, "Done" }
    }.createAndAdd(this, topSizer, wxSizerFlags().Border());

    SetSizerAndFit(topSizer);
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

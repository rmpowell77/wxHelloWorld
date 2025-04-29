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

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    auto* topSizer = new wxBoxSizer(wxVERTICAL);

    auto* sizerUpper = new wxBoxSizer(wxHORIZONTAL);
    auto upperWidgets = std::tuple {
        Widget<wxButton> { wxID_ANY, "Click" },
        Widget<wxTextCtrl> { wxID_ANY, "Dog", wxSizerFlags(1).Border() },
    };
    std::apply([this, sizer = sizerUpper](auto&&... tupleArg) {
        (tupleArg.createAndAdd(this, sizer, wxSizerFlags().Border()), ...);
    },
        upperWidgets);

    topSizer->Add(sizerUpper, wxSizerFlags().Border().Expand());

    auto* sizerLower = new wxBoxSizer(wxHORIZONTAL);
    auto lowerWidgets = std::tuple {
        Widget<wxStaticText> { wxID_ANY, "Cat" },
        Widget<wxButton> { wxID_EXIT, "Done" },
    };
    std::apply([this, sizer = sizerLower](auto&&... tupleArg) {
        (tupleArg.createAndAdd(this, sizer, wxSizerFlags().Border()), ...);
    },
        lowerWidgets);

    topSizer->Add(sizerLower, wxSizerFlags().Border());

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

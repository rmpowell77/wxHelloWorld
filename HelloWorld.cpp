// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

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

template <typename Widget>
void CreateAndAdd(std::string str, wxWindow* parent, wxSizer* sizer, wxSizerFlags flags)
{
    sizer->Add(new Widget(parent, wxID_ANY, str), flags);
}

}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    using namespace DeclarativeUI;
    // Create and layout the controls.
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* sizerText = new wxBoxSizer(wxHORIZONTAL);
    CreateAndAdd<wxTextCtrl>("Dog", this, sizerText, wxSizerFlags(1).Expand().Border());
    CreateAndAdd<wxButton>("Right", this, sizerText, wxSizerFlags().Expand().Border());

    sizer->Add(sizerText, wxSizerFlags().Expand().Border());

    auto* sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    CreateAndAdd<wxButton>("Left", this, sizerBtns, wxSizerFlags().Expand().Border());
    CreateAndAdd<wxStaticText>("Cat", this, sizerBtns, wxSizerFlags().Expand().Border());

    sizer->Add(sizerBtns, wxSizerFlags().Expand().Border());

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

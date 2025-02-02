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
    CreateStatusBar(1);
    // Create the controls.
    auto* button = new wxButton(this, wxID_ANY, "Click");
    auto* text1 = new wxTextCtrl(this, wxID_ANY, "Dog");
    auto* text2 = new wxStaticText(this, wxID_ANY, "Cat");
    auto* done = new wxButton(this, wxID_EXIT, "Done");

    // Layout the controls.
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(button, wxSizerFlags().Border());
    sizerTop->Add(text1, wxSizerFlags(1).Border());

    sizer->Add(sizerTop, wxSizerFlags().Border().Expand());

    auto* sizerBottom = new wxBoxSizer(wxHORIZONTAL);
    sizerBottom->Add(text2, wxSizerFlags().Border());
    sizerBottom->Add(done, wxSizerFlags().Border());

    sizer->Add(sizerBottom, wxSizerFlags().Border());

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

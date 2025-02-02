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

template <typename Widget>
void CreateAndAdd(wxWindow* parent, wxWindowID id, std::string str, wxSizer* sizer, wxSizerFlags flags)
{
    sizer->Add(new Widget(parent, id, str), flags);
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    auto* topSizer = new wxBoxSizer(wxVERTICAL);

    auto* sizerUpper = new wxBoxSizer(wxHORIZONTAL);
    CreateAndAdd<wxButton>(this, wxID_ANY, "Click", sizerUpper, wxSizerFlags().Border());
    CreateAndAdd<wxTextCtrl>(this, wxID_ANY, "Dog", sizerUpper, wxSizerFlags(1).Border());

    topSizer->Add(sizerUpper, wxSizerFlags().Border().Expand());

    auto* sizerLower = new wxBoxSizer(wxHORIZONTAL);
    CreateAndAdd<wxStaticText>(this, wxID_ANY, "Cat", sizerLower, wxSizerFlags().Border());
    CreateAndAdd<wxButton>(this, wxID_EXIT, "Done", sizerLower, wxSizerFlags().Border());

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

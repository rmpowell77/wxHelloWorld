// wxWidgets "Hello world" Program
#include <wx/wx.h>

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

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
        "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    // Create the controls.  This is cribbed from the RichTipDialog example
    wxStaticText* text = new wxStaticText(this, wxID_ANY, "Example of Text in wxWidgets");
    wxTextCtrl* textTitle = new wxTextCtrl(this, wxID_ANY, "Single line of text");
    wxTextCtrl* textBody = new wxTextCtrl(this, wxID_ANY,
        "Several lines of text.\n"
        "With wxUI the code reflects\n"
        "what the UI looks like.",
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    wxButton* btnLeft = new wxButton(this, wxID_ANY, "Left");
    wxButton* btnRight = new wxButton(this, wxID_ANY, "Right");

    const wxString levels[] = {
        "&Information",
        "&Warning",
        "&Error",
        "&None",
        "&Custom",
    };
    wxRadioBox* logLevels = new wxRadioBox(
        this, wxID_ANY, "&Log Levels:", wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(levels), levels, 1, wxRA_SPECIFY_ROWS);
    logLevels->SetSelection(1);

    wxCheckBox* checkBox = new wxCheckBox(this, wxID_ANY, "Show");
    const wxString choices[] = {
        "Less",
        "More",
    };

    wxChoice* choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, WXSIZEOF(choices), choices);
    wxTextCtrl* blankLine = new wxTextCtrl(this, wxID_ANY, "Fill in the blank");

    textBody->SetMinSize(wxSize(200, 100));

    // Layout the controls.
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* sizerText = new wxStaticBoxSizer(wxVERTICAL, this, "Text Examples");
    sizerText->Add(text, wxSizerFlags().Expand().Border());
    sizerText->Add(textTitle, wxSizerFlags().Expand().Border());
    sizerText->Add(textBody, wxSizerFlags(1).Expand().Border());
    sizer->Add(sizerText, wxSizerFlags(1).Expand().Border());

    sizer->Add(logLevels, wxSizerFlags().Expand().Border());

    wxStaticBoxSizer* sizerDetails = new wxStaticBoxSizer(wxHORIZONTAL, this, "Details");
    sizerDetails->Add(checkBox, wxSizerFlags().Border());
    sizerDetails->Add(choice, wxSizerFlags().Border());
    sizerDetails->Add(blankLine, wxSizerFlags(1).Expand().Border());
    sizer->Add(sizerDetails, wxSizerFlags().Expand().Border());

    wxBoxSizer* sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(btnLeft, wxSizerFlags().Border(wxLEFT));
    sizerBtns->Add(btnRight, wxSizerFlags().Border(wxRIGHT));
    sizer->Add(sizerBtns, wxSizerFlags().Centre().Border());

    SetSizerAndFit(sizer);

    // And connect the event handlers.
    btnLeft->Bind(wxEVT_BUTTON, [](wxEvent&) { wxLogMessage("Pressed Left"); });
    btnRight->Bind(wxEVT_BUTTON, [](wxEvent&) { wxLogMessage("Pressed Right"); });
}

void MyFrame::OnExit([[maybe_unused]] wxCommandEvent& event) { Close(true); }

void MyFrame::OnAbout([[maybe_unused]] wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets' Hello world sample", "About Hello World",
        wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello([[maybe_unused]] wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

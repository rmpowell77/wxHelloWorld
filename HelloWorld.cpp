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
    explicit Widget(wxWindowID id)
        : id_(id)
    {
    }

    auto createAndAdd(wxWindow* parent, wxSizer* sizer, wxSizerFlags suppliedFlags)
    {
        sizer->Add(create(parent, id_, pos_, size_, style_), flags_.value_or(suppliedFlags));
    }

    auto withFlags(wxSizerFlags flags) -> W&
    {
        flags_ = flags;
        return static_cast<W&>(*this);
    }

    auto withPos(wxPoint pos) -> W&
    {
        pos_ = pos;
        return static_cast<W&>(*this);
    }

    auto withSize(wxSize size) -> W&
    {
        size_ = size;
        return static_cast<W&>(*this);
    }

    auto withWidth(int width) -> W&
    {
        size_.SetWidth(width);
        return static_cast<W&>(*this);
    }

    auto withHeight(int height) -> W&
    {
        size_.SetHeight(height);
        return static_cast<W&>(*this);
    }

    auto withStyle(long style) -> W&
    {
        style_ = style;
        return static_cast<W&>(*this);
    }

private:
    virtual auto create(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size, long style) -> wxWindow* = 0;

    wxWindowID id_ { wxID_ANY };
    wxPoint pos_ { wxDefaultPosition };
    wxSize size_ { wxDefaultSize };
    long style_ { 0 };
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

    Sizer(wxOrientation orientation, W... widgets)
        : orientation(orientation)
        , widgets(std::make_tuple(widgets...))
    {
    }

    auto createAndAdd(wxWindow* parent, wxSizerFlags parentFlags)
    {
        auto* sizer = new wxBoxSizer(orientation);
        ::createAndAdd(parent, sizer, flags.value_or(parentFlags), widgets);
        return sizer;
    }

    auto createAndAdd(wxWindow* parent, wxSizer* parentSizer, wxSizerFlags parentFlags)
    {
        auto* sizer = createAndAdd(parent, parentFlags);
        parentSizer->Add(sizer, parentFlags);
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

struct TextCtrl : Widget<TextCtrl> {
    using super = Widget<TextCtrl>;
    explicit TextCtrl(wxWindowID id = wxID_ANY, std::string str = std::string {})
        : super(id)
        , str_(std::move(str))
    {
    }

    explicit TextCtrl(std::string str)
        : TextCtrl(wxID_ANY, std::move(str))
    {
    }

private:
    auto create(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size, long style) -> wxWindow*
    {
        return new wxTextCtrl(parent, id, str_, pos, size, style);
    }
    std::string str_;
};

struct Button : Widget<Button> {
    using super = Widget<Button>;
    explicit Button(wxWindowID id = wxID_ANY, std::string str = std::string {})
        : super(id)
        , str_(std::move(str))
    {
    }

    explicit Button(std::string str)
        : Button(wxID_ANY, std::move(str))
    {
    }

private:
    auto create(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size, long style) -> wxWindow*
    {
        return new wxButton(parent, id, str_, pos, size, style);
    }
    std::string str_;
};

struct Text : Widget<Text> {
    using super = Widget<Text>;
    explicit Text(wxWindowID id = wxID_ANY, std::string str = std::string {})
        : super(id)
        , str_(std::move(str))
    {
    }

    explicit Text(std::string str)
        : Text(wxID_ANY, std::move(str))
    {
    }

private:
    auto create(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size, long style) -> wxWindow*
    {
        return new wxStaticText(parent, id, str_, pos, size, style);
    }
    std::string str_;
};

static_assert(CreateAndAddable<TextCtrl>);
static_assert(CreateAndAddable<Button>);
static_assert(CreateAndAddable<Text>);

static_assert(CreateAndAddable<HSizer<Button, TextCtrl>>);
static_assert(CreateAndAddable<HSizer<Text, Button>>);

static_assert(CreateAndAddable<VSizer<HSizer<Button, TextCtrl>, HSizer<Text, Button>>>);

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
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

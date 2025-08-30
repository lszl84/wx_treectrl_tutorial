#include <wx/wx.h>
#include <wx/treectrl.h>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

private:
    wxTreeCtrl *tree;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("Directory Tree Viewer", wxPoint(900, 600), wxSize(800, 600));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    tree = new wxTreeCtrl(this);

    auto root = tree->AddRoot("Root Element");
    auto child = tree->AppendItem(root, "Child 1");

    tree->AppendItem(child, "Child 2");
    tree->AppendItem(root, "Child 3");

    tree->Expand(root);
}

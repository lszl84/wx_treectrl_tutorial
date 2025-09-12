#include <filesystem>
#include <set>

#include <wx/artprov.h>
#include <wx/dirdlg.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

namespace fs = std::filesystem;

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void PopulateTree(const fs::path& path, wxTreeItemId parentId);
    void OpenFolder(const fs::path& rootPath);
    wxTreeCtrl* tree;
    wxButton* openFolderButton;

    const int folderIconIdx = 0;
    const int fileIconIdx = 1;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame("Directory Tree Viewer", wxPoint(900, 600), wxSize(800, 600));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    tree = new wxTreeCtrl(this);

    wxVector<wxBitmapBundle> images;

    images.push_back(wxArtProvider::GetBitmapBundle(wxART_FOLDER));
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE));

    tree->SetImages(images);

    openFolderButton = new wxButton(this, wxID_ANY, "Open Folder...");

    auto sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(tree, wxSizerFlags(1).Border().Expand());
    sizer->Add(openFolderButton, wxSizerFlags().Border().Center());

    SetSizerAndFit(sizer);

    std::error_code ec;
    auto currentPath = fs::current_path(ec);

    if (!ec) {
        OpenFolder(currentPath);
    }

    openFolderButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
        wxDirDialog dirDialog(this, "Select a folder to open");

        if (dirDialog.ShowModal() == wxID_OK) {
            this->OpenFolder(dirDialog.GetPath().ToStdString());
        }
    });
}

void MyFrame::PopulateTree(const fs::path& path, wxTreeItemId parentId)
{
    std::error_code ec;

    std::set sortedEntries(
        fs::directory_iterator { path, {}, ec },
        fs::directory_iterator {},
        [&ec](const auto& a, const auto& b) {
            const bool a_is_folder = a.is_directory(ec);
            const bool b_is_folder = b.is_directory(ec);

            const bool a_comes_first = a_is_folder != b_is_folder
                ? a_is_folder
                : a.path().filename() < b.path().filename();

            return a_comes_first;
        });

    for (const auto& entry : sortedEntries) {
        wxString name = entry.path().filename().string();

        if (entry.is_directory(ec)) {
            wxTreeItemId item = tree->AppendItem(parentId, name, folderIconIdx);

            if (!entry.is_symlink(ec)) {
                PopulateTree(entry.path(), item);
            }
        } else {
            if (ec) {
                ec.clear();
                continue;
            } else {
                tree->AppendItem(parentId, name, fileIconIdx);
            }
        }
    }
}

void MyFrame::OpenFolder(const fs::path& rootPath)
{
    tree->DeleteAllItems();
    tree->Refresh();
    tree->Update();

    wxTreeItemId root = tree->AddRoot(rootPath.filename().string(), folderIconIdx);

    PopulateTree(rootPath, root);

    tree->Expand(root);
}

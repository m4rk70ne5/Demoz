#include "Handlers.h"
#include "tDialog.h"
#include "tEdit.h"
#include "tButton.h"
#include "tControl.h"
#include "resource.h"

int SpawnSettingsDialog(Windows::tControl* control)
{
    using namespace Windows;

    tDialog settingsDialog(0, 0, 259, 76, L"Settings"); // in the future, grab dialog from a Dialog Store?
    // add controls to settings dialog
    settingsDialog.AddControl((tControl*)(new tButton(TEXT("&Save"), 510, 40, 116, 35, IDC_BTN_QUIT2))); // change id later
    settingsDialog.AddControl((tControl*)(new tButton(TEXT("&Cancel"), 510, 95, 116, 35, IDC_BTN_QUIT2))); // change id later
    settingsDialog.AddControl((tControl*)(new tControl(10, 30, 105, 25, TEXT("&Shader Path:"))));
    settingsDialog.AddControl((tControl*)(new tEdit(TEXT("Shader Path"), 135, 30, 300, 25, IDC_TEXT1))); // change id later
    settingsDialog.AddControl((tControl*)(new tBrowseButton(TEXT(". . ."), 445, 30, 35, 25, IDC_BTN_BROWSE1, IDC_TEXT1, SpawnFileBrowser)));
    settingsDialog.AddControl((tControl*)(new tControl(10, 75, 110, 25, TEXT("&Resource Dir:"))));
    settingsDialog.AddControl((tControl*)(new tEdit(TEXT("Texture Path"), 135, 75, 300, 25, IDC_TEXT2))); // change id later
    settingsDialog.AddControl((tControl*)(new tControl(10, 120, 105, 25, TEXT("&Output Dir:")))); // change id later
    settingsDialog.AddControl((tControl*)(new tEdit(TEXT("Output Directory"), 135, 120, 300, 25, IDC_TEXT3))); // change id later
    return settingsDialog.Show(control->GetParentWindow());
}

int SpawnFileBrowser(Windows::tControl* control)
{
    HWND parent = control->GetParentWindow();
    Windows::tOpenFolderDialog tOFD;
    tOFD.Show(parent, TEXT("Select Source Folder"));
    std::wstring path = tOFD.GetPath();
    int id = static_cast<Windows::tBrowseButton*>(control)->GetEditID();
    SetDlgItemText(parent, id, path.c_str());
    return 0;
}
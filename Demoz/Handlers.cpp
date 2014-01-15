#include "Handlers.h"
#include "tDialog.h"
#include "tEdit.h"
#include "tButton.h"
#include "tControl.h"
#include "resource.h"
#include "tDemoList.h"

int SpawnSettingsDialog(Windows::tControl* control, PVOID listBox)
{
    using namespace Windows;
	tDemoList* pDemoList = static_cast<tDemoList*>(listBox);
	int index = pDemoList->GetCurrentEntry();
	// give the pointer to the current entry's settings
	tPathSettings* pSettings = pDemoList->GetSettings(index);
    tDialog settingsDialog(0, 0, 259, 76, L"Settings"); // in the future, grab dialog from a Dialog Store?
    // add controls to settings dialog
	tButton* pSaveButton = new tButton(TEXT("&Save"), 510, 40, 116, 35, IDC_BTN_SAVE, SaveSettings);
	pSaveButton->SetAdditinalInfo(static_cast<PVOID>(pSettings));
    settingsDialog.AddControl((tControl*)pSaveButton); // change id later
    settingsDialog.AddControl((tControl*)(new tButton(TEXT("&Cancel"), 510, 95, 116, 35, IDC_BTN_QUIT))); // change id later
    settingsDialog.AddControl((tControl*)(new tControl(10, 30, 105, 25, TEXT("&Shader Path:"))));
    settingsDialog.AddControl((tControl*)(new tEdit((TCHAR*)(pSettings->shaderPath).c_str(), 135, 30, 300, 25, IDC_TEXT1))); // change id later
    settingsDialog.AddControl((tControl*)(new tBrowseButton(TEXT(". . ."), 445, 30, 35, 25, IDC_BTN_BROWSE1, IDC_TEXT1, SpawnFileBrowser)));
    settingsDialog.AddControl((tControl*)(new tControl(10, 75, 110, 25, TEXT("&Resource Dir:"))));
    settingsDialog.AddControl((tControl*)(new tEdit((TCHAR*)(pSettings->resourcePath).c_str(), 135, 75, 300, 25, IDC_TEXT2))); // change id later
	settingsDialog.AddControl((tControl*)(new tBrowseButton(TEXT(". . ."), 445, 30, 35, 25, IDC_BTN_BROWSE2, IDC_TEXT2, SpawnFileBrowser)));
    settingsDialog.AddControl((tControl*)(new tControl(10, 120, 105, 25, TEXT("&Output Dir:")))); // change id later
    settingsDialog.AddControl((tControl*)(new tEdit((TCHAR*)(pSettings->outputPath).c_str(), 135, 120, 300, 25, IDC_TEXT3))); // change id later
	settingsDialog.AddControl((tControl*)(new tBrowseButton(TEXT(". . ."), 445, 30, 35, 25, IDC_BTN_BROWSE3, IDC_TEXT3, SpawnFileBrowser)));
    return settingsDialog.Show(control->GetParentWindow());
}

int SpawnFileBrowser(Windows::tControl* control, PVOID unused)
{
    HWND parent = control->GetParentWindow();
    Windows::tOpenFolderDialog tOFD;
	// try to get the initial path
	Windows::tBrowseButton* pButton = static_cast<Windows::tBrowseButton*>(control);
	DWORD connectedEdit = pButton->GetEditID();
	TCHAR* initPath = new TCHAR[255];
	GetDlgItemText(pButton->GetParentWindow(), connectedEdit, initPath, 255);
    tOFD.Show(parent, TEXT("Select Source Folder"), static_cast<LPWSTR>(initPath));
    std::wstring path = tOFD.GetPath();
    int id = static_cast<Windows::tBrowseButton*>(control)->GetEditID();
    SetDlgItemText(parent, id, path.c_str());
	delete initPath; // free allocated memory
    return 0;
}

int RunDemo(Windows::tControl* control, PVOID listBox)
{
	tDemoList* pDemoList = static_cast<tDemoList*>(listBox);
	int index = pDemoList->GetCurrentEntry();
	// disable the run button
	pDemoList->RunEntry(index); // run the demo
}

int SaveSettings(Windows::tControl* control, PVOID settings)
{
	tPathSettings* pSettings = static_cast<tPathSettings*>(settings);
	// get the values from each edit
	TCHAR* path = (TCHAR*)malloc(sizeof(TCHAR) * 255);
	GetDlgItemText(control->GetParentWindow(), IDC_TEXT1, path, 255);
	pSettings->shaderPath = wstring(path);
	GetDlgItemText(control->GetParentWindow(), IDC_TEXT2, path, 255);
	pSettings->resourcePath = wstring(path);
	GetDlgItemText(control->GetParentWindow(), IDC_TEXT3, path, 255);
	pSettings->outputPath = wstring(path);
	// free the memory
	free(path);
}
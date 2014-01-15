#include "resource.h"
#include "tDialog.h"
#include "Handlers.h"
#include "tButton.h"
#include "tDemoList.h"

HINSTANCE hInst = NULL;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst = hInstance;
    LPCWSTR pszTitle = L"3D Graphics Demo Framework";

    using namespace Windows;
    tDialog mainDialog(6, 5, 424, 176, pszTitle);
	tDemoList* pDemoList = new tDemoList("demoz.dsf", TEXT("Demo List"), 11, 10, 748, 310, IDC_LIST);
	tButton* pRunButton = new tButton(TEXT("&Run"), 228, 350, 136, 45, IDC_BTN_RUN, RunDemo);
	pRunButton->SetAdditinalInfo(dynamic_cast<PVOID>(pDemoList));
    mainDialog.AddControl((tControl*)pRunButton);
    mainDialog.AddControl((tControl*)new tButton(TEXT("&Stop"), 419, 350, 136, 45, IDC_BTN_STOP));
    tButton* pSettingsButton = new tButton(TEXT("&Settings..."), 830, 290, 126, 35, IDC_BTN_SETTINGS, SpawnSettingsDialog);
	pSettingsButton->SetAdditinalInfo(dynamic_cast<PVOID>(pDemoList));
	mainDialog.AddControl((tControl*)pSettingsButton);
	
    mainDialog.AddControl((tControl*)pDemoList);
    int ret = mainDialog.Show((HWND)NULL);
    return ret;
}
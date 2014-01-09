#include "resource.h"
#include "tDialog.h"
#include "Handlers.h"
#include "tButton.h"
#include "tListBox.h"

HINSTANCE hInst = NULL;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst = hInstance;
    LPCWSTR pszTitle = L"3D Graphics Demo Framework";

    using namespace Windows;
    tDialog mainDialog(6, 5, 424, 176, pszTitle);
    mainDialog.AddControl((tControl*)new tButton(TEXT("&Run"), 228, 350, 136, 45, IDC_BTN_RUN));
    mainDialog.AddControl((tControl*)new tButton(TEXT("&Quit"), 419, 350, 136, 45, IDC_BTN_QUIT));
    mainDialog.AddControl((tControl*)new tButton(TEXT("&Settings..."), 830, 290, 126, 35, IDC_BTN_SETTINGS, SpawnSettingsDialog));
    mainDialog.AddControl((tControl*)new tListBox(TEXT("Demo List"), 11, 10, 748, 310, IDC_LIST));
    int ret = mainDialog.Show((HWND)NULL);
    return ret;
}
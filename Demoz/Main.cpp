#include "resource.h"
#include "tDialog.h"
#include "Handlers.h"
#include "tButton.h"
#include "tDemoList.h"
#include "tGraphicsController.h"
#include "tInputController.h"
#include "tProgramManager.h"

HINSTANCE hInst = NULL;

void Cleanup()
{
	// deallocate controllers and other singletons
	tGraphicsController* pGC = tGraphicsController::GetGraphicsController();
	if (pGC != NULL)
	{
		delete pGC;
	}
	tInputController* pIC = tInputController::GetInputController();
	if (pIC != NULL)
	{
		delete pIC;
	}
	tProgramManager* pPM = tProgramManager::GetProgramManager();
	if (pPM != NULL)
	{
		delete pPM;
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst = hInstance;
    LPCWSTR pszTitle = L"3D Graphics Demo Framework";
	
	int ret;
	{
    using namespace Windows;
    tDialog mainDialog(6, 5, 424, 176, pszTitle);
	tDemoList* pDemoList = new tDemoList("demoz.dsf", TEXT("Demo List"), 11, 10, 748, 310, IDC_LIST);
	tButton* pRunButton = new tButton(TEXT("&Run"), 228, 350, 136, 45, IDC_BTN_RUN, RunDemo);
	pRunButton->SetAdditinalInfo(dynamic_cast<PVOID>(pDemoList));
    mainDialog.AddControl((tControl*)pRunButton);
	tButton* pStopButton = new tButton(TEXT("&Stop"), 419, 350, 136, 45, IDC_BTN_STOP, StopDemo, TRUE);
	pStopButton->SetAdditinalInfo(dynamic_cast<PVOID>(pDemoList));
    mainDialog.AddControl((tControl*)pStopButton);
    tButton* pSettingsButton = new tButton(TEXT("&Settings..."), 830, 290, 126, 35, IDC_BTN_SETTINGS, SpawnSettingsDialog);
	pSettingsButton->SetAdditinalInfo(dynamic_cast<PVOID>(pDemoList));
	mainDialog.AddControl((tControl*)pSettingsButton);
	
    mainDialog.AddControl((tControl*)pDemoList);
    ret = mainDialog.Show((HWND)NULL);
	} // these scope braces will automatically deallocate objects attached to mainDialog

	// clean up
	Cleanup();
    return ret;
}
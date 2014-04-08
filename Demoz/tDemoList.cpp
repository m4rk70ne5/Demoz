#include "tDemoList.h"
#include "tDSFFile.h"
#include "resource.h"
#include <process.h>

tDemoListEntry::tDemoListEntry(wstring title, tDemo*(*create)(), int width, int height) : m_name(title), m_creation(create), 
	m_width(width), m_height(height)
{
}

tDemoList::tDemoList(string settingsFile, TCHAR* title, int x, int y, int w, int h, DWORD id) : m_settingsFile(settingsFile),
	tListBox(title, x, y, w, h, id)
{
	// create the demos
	CreateDemos();
	// new file reader
	m_fileIO = new tDSFFile(settingsFile);
}

tDemoList::~tDemoList()
{
	// save the demos to file
	m_fileIO->WriteDemos(m_entries);
	// now destroy the demos
	for (vector<tDemoListEntry*>::iterator iVDemos = m_entries.begin(); iVDemos != m_entries.end(); ++iVDemos)
	{
		delete *iVDemos;
	}
	delete m_fileIO;
}

void tDemoList::CreateDemos()
{
	Add(CreateDemo<tBSPDemo>, L"BSP Viewer", 800, 600);
	Add(CreateDemo<tSpinningBoxDemo>, L"Spinning Boxes", 800, 600);
}

void tDemoList::Add(tDemo*(*createMe)(), TCHAR* name, int width, int height)
{
	// don't call the creation function yet -- only right before running
	// you don't want to create demos in memory that aren't ever ran
	m_entries.push_back(new tDemoListEntry(name, createMe, width, height));
	m_entries.back()->m_settings = m_fileIO->ReadSettings(m_entries.back()->m_name, m_settingsFile); // load settings
}

void tDemoList::CreateMe(HWND hwnd)
{
	// call base class createme
	tListBox::CreateMe(hwnd);

	// clear the list box
	ClearAll();

	// now cycle through the list
	for (vector<tDemoListEntry*>::iterator iVDemos = m_entries.begin(); iVDemos != m_entries.end(); ++iVDemos)
	{
		// index in the vector will correspond to index in the list
		SendMessage(m_hwnd, LB_ADDSTRING, 0, (LPARAM)((*iVDemos)->m_name.c_str()));
	}
}

void tDemoList::RunEntry(int index)
{
	tDemoListEntry& entry = *m_entries[index];
	// disable the selection changing and double-clicking
	// create
	tDemo* pDemo = (tDemo*)(entry.m_creation());
	entry.m_demo = pDemo;
	pDemo->m_parentListBox = this;
	pDemo->m_settings = entry.m_settings; // scan file for settings, using m_name as the key
	pDemo->m_title = string(entry.m_name.begin(), entry.m_name.end());
	pDemo->SetDimensions(entry.m_width, entry.m_height);
	entry.m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, StartDemoThread, dynamic_cast<void*>(pDemo), 0, NULL); // start pDemo in a new thread
	EnableWindow(m_hwnd, FALSE); // disable the list for keyboard and mouse input
	// when thread returns...
	// re-enable selection changing and double-clicking
	// enable run, disable stop
	// delete pDemo
}

void tDemoList::SendStopSignal(int index)
{
	// disable the stop button
	EnableWindow(GetDlgItem(GetParentWindow(), IDC_BTN_STOP), FALSE);
	tDemoListEntry& entry = *m_entries[index];
	tDemo* pDemo = entry.m_demo;
	pDemo->Stop();
	// now wait until the thread quits to deallocate
	DWORD exitCode;
	int i;
	GetExitCodeThread(entry.m_threadHandle, &exitCode);
	for (i = 0;  exitCode == STILL_ACTIVE && i < 10; ++i)
	{
		// wait for the thread to return
		Sleep(500);
		GetExitCodeThread(entry.m_threadHandle, &exitCode);
	}
	if (i == 20)
	{
		// kill the thread manually
		TerminateThread(entry.m_threadHandle, -1);
	}
	// restore listbox and windows
	EnableWindow(GetHwnd(), FALSE); // disable the list for keyboard and mouse input
	// disable stop, enable run
	EnableWindow(GetDlgItem(GetParentWindow(), IDC_BTN_RUN), TRUE);
	// enable settings
	EnableWindow(GetDlgItem(GetParentWindow(), IDC_BTN_SETTINGS), TRUE);
	// enable the demo list again
	delete pDemo;
}

unsigned __stdcall tDemoList::StartDemoThread(void* demo)
{
	tDemo* pDemo = static_cast<tDemo*>(demo);
	pDemo->Start();
	return 0;
}
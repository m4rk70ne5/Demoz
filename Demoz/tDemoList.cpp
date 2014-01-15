#include "tDemoList.h"
#include "tDSFFile.h"

tDemoList::~tDemoList()
{
	for (vector<tDemoListEntry*>::iterator iVDemos = m_entries.begin(); iVDemos != m_entries.end(); ++iVDemos)
	{
		delete *iVDemos;
	}
}

void tDemoList::Add(tDemo*(*createMe)(), TCHAR* name)
{
	// don't call the creation function yet -- only right before running
	// you don't want to create demos in memory that aren't ever ran
	m_entries.push_back(new tDemoListEntry(name, createMe));
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
	pDemo->m_settings = m_fileIO->ReadSettings(entry.m_name, m_settingsFile); // scan file for settings, using m_name as the key
	pDemo->m_title = string(entry.m_name.begin(), entry.m_name.end());
	_beginthreadex(NULL, 0, pDemo->Start(), (PVOID*)0, &entry.m_threadID); // start pDemo in a new thread

	// when thread returns...
	// re-enable selection changingand double-clicking
	// enable run, disable stop
	// delete pDemo
}
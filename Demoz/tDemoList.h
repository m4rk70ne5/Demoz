#ifndef TDEMOLIST_H
#define TDEMOLIST_H

#include "tListBox.h"
#include "tDemo.h"
#include "tBSPDemo.h"
#include "tOctreeDemo.h"
#include <string>
#include <vector>

using namespace std;

class tDSFFile;

class tDemoListEntry
{
	// essentially a demo proxy
public:
	tDemoListEntry(wstring, tDemo*(*)(), int, int);
	int m_width, m_height;
	wstring m_name;
	tPathSettings m_settings;
	HANDLE m_threadHandle;
	tDemo* (*m_creation)();
	tDemo* m_demo;
};

class tDemoList : public Windows::tListBox
{
public:
	tDemoList(string, TCHAR*, int, int, int, int, DWORD); // name of the settings file (or sqlite database)
	virtual ~tDemoList();
	void Add(tDemo*(*)(), TCHAR*, int, int);  // creation function and name
	virtual void CreateMe(HWND);
	void RunEntry(int);
	tPathSettings* GetSettings(int index) { return &(m_entries[index]->m_settings); };
	void SendStopSignal(int);
protected:
	tDSFFile* m_fileIO;
	string m_settingsFile;
	vector<tDemoListEntry*> m_entries;
	template<typename T>
	static tDemo* CreateDemo()
	{
		return new T();
	};
	static unsigned __stdcall StartDemoThread(void*);
	void CreateDemos(); // list of demos
};

#endif
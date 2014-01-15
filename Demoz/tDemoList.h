#ifndef TDEMOLIST_H
#define TDEMOLIST_H

#include "tListBox.h"
#include "tDemo.h"
#include <string>
#include <vector>

using namespace std;

class tDSFFile;

class tDemoListEntry
{
	// essentially a demo proxy
public:
	tDemoListEntry(wstring, tDemo*(*)());
	wstring m_name;
	tPathSettings m_settings;
	DWORD m_threadID;
	tDemo* (*m_creation)();
};

class tDemoList : public Windows::tListBox
{
public:
	tDemoList(string, TCHAR*, int, int, int, int, DWORD); // name of the settings file (or sqlite database)
	virtual ~tDemoList();
	void Add(tDemo*(*)(), TCHAR*);  // creation function and name
	virtual void CreateMe(HWND);
	void RunEntry(int);
	tPathSettings* GetSettings(int index) { return &(m_entries[index]->m_settings); };
protected:
	tDSFFile* m_fileIO;
	string m_settingsFile;
	vector<tDemoListEntry*> m_entries;
	template<class T>
	T* CreateDemo()
	{
		return new T();
	};
};

#endif
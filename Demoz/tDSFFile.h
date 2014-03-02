#ifndef TDSFFILE_H
#define TDSFFILE_H

#include <string>
#include <map>
#include <fstream>
#include <vector>
#include "tDemo.h"

using namespace std;

class tDemoListEntry;

class tDSFFile
{
public:
	tDSFFile(string file) : m_filename(file) { };
	tPathSettings ReadSettings(wstring, string);
	void WriteDemos(vector<tDemoListEntry*>&);
protected:
	string m_filename;
	int FillHeader(ifstream*, pair<map<wstring, int>, int>&); // returns the offset of the body
	tPathSettings GetSettings(ifstream*, int); // gets settings at offset
	void WriteSettingsStruct(wstring, tPathSettings&);
	wstring NextString(istream*);
	void WriteString(ofstream*, wstring);
	int InsertNewDemo(ofstream*, ofstream*, wstring, tPathSettings&, int); // inserts new demo in file
	void FinalizeWrite(ofstream*, ofstream*);
};

#endif
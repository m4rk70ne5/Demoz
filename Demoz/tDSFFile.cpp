#include "tDSFFile.h"
#include "tDemoList.h"

tPathSettings tDSFFile::ReadSettings(wstring key, string settingsFile)
{
	// open file
	std::ifstream file;
	file.open(settingsFile, ifstream::binary);
	// the directory
	pair<map<wstring, int>, int> demoOffset;
	int bodyOffset = FillHeader(&file, demoOffset);
	if (bodyOffset >= 0)
	{
		tPathSettings settings = GetSettings(&file, demoOffset.first[key] + demoOffset.second);
		// search for the key
		file.close();
		return settings;
	}
	else
	{
		return tPathSettings();
	}
}

int tDSFFile::FillHeader(ifstream* pFile, pair<map<wstring, int>, int>& offsetMap)
{
	// skip the file signature
	char* buffer = (char*)malloc(256);
	pFile->read(buffer, sizeof(char)*2);
	string signature(buffer, 2);
	if (!signature.compare("DZ"))
	{
		// continue on if they match
		// read in header and body offset
		int bodyOffset;
		pFile->read((char*)&bodyOffset, sizeof(int));
		offsetMap.second = bodyOffset;
		// now read in the header
		// header should be right after body offset
		while (pFile->tellg() < bodyOffset)
		{
			wstring demoName = NextString(pFile);
			int demoOffset;
			pFile->read((char*)&demoOffset, sizeof(int));
			offsetMap.first[demoName] = demoOffset;
		}
		free(buffer);
		return bodyOffset;
	}
	free(buffer);
	return -1;
}

void tDSFFile::WriteString(ofstream* pFile, wstring name)
{
	unsigned short size = name.length();
	pFile->write((char*)&size, sizeof(unsigned short));
	pFile->write((char*)name.c_str(), size * sizeof(wchar_t));
}

wstring tDSFFile::NextString(istream* pFile)
{
	// read in size
	unsigned short size; // number of characters without null
	pFile->read((char*)&size, sizeof(unsigned short));
	wchar_t* buf = new wchar_t[size + 1]; // + 1 for nt
	pFile->read((char*)buf, sizeof(wchar_t) * size);
	buf[size] = '\0';
	wstring retString(buf);
	delete buf;
	return retString;
}

tPathSettings tDSFFile::GetSettings(ifstream* pFile, int offset)
{
	pFile->seekg(offset);
	// now read in the next three strings
	tPathSettings settings;
	wstring shader = NextString(pFile);
	settings.shaderPath = shader;
	wstring resource = NextString(pFile);
	settings.resourcePath = resource;
	wstring output = NextString(pFile);
	settings.outputPath = output;
	return settings;
}

int tDSFFile::InsertNewDemo(ofstream* outfile, ofstream* outfile2, wstring key, tPathSettings& settings, int bodyOffset)
{
	// write demo name to first file
	WriteString(outfile, key);
	// write the body offset
	outfile->write((char*)&bodyOffset, sizeof(int));
	// write settings to second file
	WriteString(outfile2, settings.shaderPath);
	WriteString(outfile2, settings.resourcePath);
	WriteString(outfile2, settings.outputPath);
	return outfile2->tellp(); // the beginning of next demo settings
}

void tDSFFile::WriteDemos(vector<tDemoListEntry*>& demos)
{
	ofstream out1, out2;
	out1.open(m_filename, ofstream::binary);
	out2.open("temp.bin", ofstream::binary);
	// write signatures and offset int to out1
	int temp;
	out1.write("DZ", 2);
	out1.write((char*)&temp, sizeof(int));
	int offset = 0;
	for (vector<tDemoListEntry*>::iterator iDemos = demos.begin(); iDemos != demos.end(); ++iDemos)
	{
		offset = InsertNewDemo(&out1, &out2, (*iDemos)->m_name, (*iDemos)->m_settings, offset);
	}
	FinalizeWrite(&out1, &out2);
}

void tDSFFile::FinalizeWrite(ofstream* first, ofstream* second)
{
	int bodyOffset = first->tellp();
	first->seekp(2);
	first->write((char*)&bodyOffset, sizeof(int));
	// now concatenate the second file to the first
	second->close();
	ifstream tempInput;
	tempInput.open("temp.bin", ifstream::binary);
	while (tempInput.good())
	{
		char c;
		c = tempInput.get();
		if (tempInput.good())
		{
			// write to end of first file
			first->write(&c, 1);
		}
	}
	tempInput.close();
	second->close();
}
#ifndef TDEMO_H
#define TDEMO_H

#include <string>

using namespace std;

struct tPathSettings
{
	tPathSettings()
	{
		shaderPath = wstring(L"Shader Path");
		resourcePath = wstring(L"Resource Path");
		outputPath = wstring(L"Output Path");
	};
	wstring shaderPath;
	wstring resourcePath;
	wstring outputPath;
};

class tDemo
{
public:
	tPathSettings m_settings;
	string m_title;
	virtual void Start(); // "game loop" goes here
};

// create demos here

#endif
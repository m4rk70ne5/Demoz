#ifndef TDEMO_H
#define TDEMO_H

#include <Windows.h>
#include <string>
#include <SDL2/SDL.h>

using namespace std;

class tDemoList;

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
protected:
	bool m_done;
	int m_width, m_height; // of the window
	SDL_Window* m_window; // every demo has a window
	SDL_GLContext m_context;
	virtual void InitDemo();
	virtual void InitSubsystem();
	virtual void EndDemo();
	static unsigned __stdcall StopDemoThread(void*);
public:
	tDemoList* m_parentListBox;
	tPathSettings m_settings;
	string m_title;
	tDemo() { m_done = false; };
	virtual ~tDemo();
	tDemo(const tDemo&) {};
	void Start(); // "game loop" goes here
	void SetDimensions(int width, int height) { m_width = width; m_height = height; };
	void Stop() { m_done = true; };
};

class tSpinningBoxDemo : public tDemo
{
public:
	tSpinningBoxDemo() : tDemo() {};
};

// create demos here

#endif
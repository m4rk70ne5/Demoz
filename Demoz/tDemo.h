#ifndef TDEMO_H
#define TDEMO_H

#include <Windows.h>
#include <string>
#include <SDL2/SDL.h>
#include <vector>
#include <map>

using namespace std;

class tDemoList;
class tGeometry;
class tGraphicsController;
class tInputController;
class tTextureManager;
class tObject;
class tCamera;
class tGameClock;

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
	virtual void InitDemo(int = 0, int = 0);
	virtual void EndDemo();
	static unsigned __stdcall StopDemoThread(void*);
	static tGraphicsController* m_tGC;
	static tInputController* m_IC;
	static tTextureManager* m_tTM;
	int m_width, m_height;
	map<int, vector<tGeometry*>> m_geoContainer;
	map<int, string> m_vaoProgram;
	string ResString(char*);
	string ShadString(char*);
	vector<tObject*> m_dynamicObj;
	tGameClock* m_gameClock;
public:
	tDemoList* m_parentListBox;
	tPathSettings m_settings;
	string m_title;
	tDemo() { m_done = false; };
	virtual ~tDemo();
	tDemo(const tDemo&) {};
	void Start(); // "game loop" goes here
	void SetDimensions(int width, int height) {m_width = width; m_height = height;};
	void Stop();
};

class tSpinningBoxDemo : public tDemo
{
public:
	tSpinningBoxDemo() : tDemo() {};
	virtual void InitDemo(int = 0, int = 0);
	virtual void EndDemo();
	tCamera* m_Camera;
};

// create demos here

#endif
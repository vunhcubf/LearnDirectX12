#pragma once
#include "MyWindow.h"
#include <vector>
#include <windowsx.h>
#include <sstream>
#include "KeyBoard.h"
#include "Graphics.h"
#include "GameTimer.h"

class App {
public:
	App(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int mCmdShow) :hInstance(hInstance), hPrevInstance(hPrevInstance), lpCmdLine(lpCmdLine), mCmdShow(mCmdShow) {};
	void Initlize();
	void DoFrame();
	void Quit();
	void ProcessMessage();
	int GetWindowsCount()const { return windows.size(); }
	int QuitAppCounter=0;
	std::vector<MyWindow*> GetWindows()const;
private:
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR lpCmdLine;
	int mCmdShow;
	std::vector<MyWindow*> windows;
	std::vector<Graphics*> graphics;
};
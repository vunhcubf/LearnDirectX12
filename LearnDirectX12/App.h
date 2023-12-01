#pragma once
#include "MyWindow.h"
#include <vector>
#include <windowsx.h>
#include <sstream>
#include "KeyBoard.h"
#include "Graphics.h"
#include "Camera.h"
#include "GameTimer.h"

class App {
public:
	App(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int mCmdShow) :hInstance(hInstance), hPrevInstance(hPrevInstance), lpCmdLine(lpCmdLine), mCmdShow(mCmdShow) {};
	void Initlize();
	void DoFrame();
	void Quit();
	void ProcessMessage();
	int QuitAppCounter=0;
	MyWindow* GetWindows()const;

	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR lpCmdLine;
	int mCmdShow;
	MyWindow* windows;
	Graphics* graphics;
	Camera camera;
	GameTimer timer;
};
#pragma once
#include <vector>
#include <windowsx.h>
#include <sstream>
#include "Graphics.h"
#include "GameTimer.h"
#include "Mesh.h"
#include "Material.h"
#include "Renderer.h"
#include "ConstantBuffer.h"
#include "ConstantBuffer.cpp"

class App {
public:
	App(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int mCmdShow) :hInstance(hInstance), hPrevInstance(hPrevInstance), lpCmdLine(lpCmdLine), mCmdShow(mCmdShow) {};
	void Initlize();
	void DoFrame();
	void Quit();
	void ProcessMessage();
	int QuitAppCounter=0;
	MyWindow* GetWindows()const;

	WindowsEventManager* eventManager;
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR lpCmdLine;
	int mCmdShow;

	MyWindow* windows;
	Graphics* graphics;
	Camera camera;
	GameTimer timer;
	Texture* chicken;

	ConstantBuffer<XMFLOAT4X4>* CBufferPerFrame;
	ComPtr<ID3DBlob> ByteCodeVS;
	ComPtr<ID3DBlob> ByteCodePS;
	Mesh* mesh;
	Material* mat;
	Renderer* renderer;
};
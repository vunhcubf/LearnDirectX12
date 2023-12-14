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
#include <unordered_map>
#include "Console.h"
#include <filesystem>
#include <array>

class App {
public:
	App(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int mCmdShow) :hInstance(hInstance), hPrevInstance(hPrevInstance), lpCmdLine(lpCmdLine), mCmdShow(mCmdShow) {};
	void Initlize();
	void DoFrame();
	void Quit();
	void ProcessMessage();
	int QuitAppCounter=0;
	//窗口类
	MyWindow* windows;
	MyWindow* GetWindows()const;
	//当前进程的变量
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR lpCmdLine;
	int mCmdShow;
	//状态控制台
	Console StatusConsole;
	//事件管理器
	WindowsEventManager* eventManager;
	//绘图基础设施
	Graphics* graphics;
	Camera camera;
	GameTimer timer;

	ConstantBuffer<XMFLOAT4X4>* CBufferPerFrame;
	//渲染器，高级抽象
	Renderer* renderer;

	std::unordered_map<std::wstring, ComPtr<ID3DBlob>> ShaderVSCollection;
	std::unordered_map<std::wstring, ComPtr<ID3DBlob>> ShaderPSCollection;
	std::unordered_map<std::wstring, Material*> MaterialCollection;
	std::unordered_map<std::wstring, Mesh*> MeshCollection;
	std::unordered_map<std::wstring, Texture*> TextureCollection;
	std::unordered_map<std::wstring, Object*> ObjectCollection;
};
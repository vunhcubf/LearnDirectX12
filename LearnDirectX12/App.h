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
	//������
	MyWindow* windows;
	MyWindow* GetWindows()const;
	//��ǰ���̵ı���
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR lpCmdLine;
	int mCmdShow;
	//״̬����̨
	Console StatusConsole;
	//�¼�������
	WindowsEventManager* eventManager;
	//��ͼ������ʩ
	Graphics* graphics;
	Camera camera;
	GameTimer timer;

	ConstantBuffer<XMFLOAT4X4>* CBufferPerFrame;
	//��Ⱦ�����߼�����
	Renderer* renderer;

	std::unordered_map<std::wstring, ComPtr<ID3DBlob>> ShaderVSCollection;
	std::unordered_map<std::wstring, ComPtr<ID3DBlob>> ShaderPSCollection;
	std::unordered_map<std::wstring, Material*> MaterialCollection;
	std::unordered_map<std::wstring, Mesh*> MeshCollection;
	std::unordered_map<std::wstring, Texture*> TextureCollection;
	std::unordered_map<std::wstring, Object*> ObjectCollection;
};
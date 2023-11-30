#pragma once
#include <DirectXColors.h>
#include "MyWindow.h"
#include <dxgi.h>
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN // 从 Windows 头中排除极少使用的资料
#include <windows.h>
#include <tchar.h>
//添加WTL支持 方便使用COM
#include <wrl.h>
using namespace Microsoft;
using namespace Microsoft::WRL;
#include <dxgi1_6.h>
#include <DirectXMath.h>
using namespace DirectX;
//for d3d12
#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
//linker
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif

#include "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\ProjectTemplates\VC\Windows UAP\2052\DirectX12UAP\d3dx12.h"

class Graphics {
public:
	Graphics(MyWindow* Wnd);
	~Graphics() {
		if (pID3DDevice != nullptr) { FlushCommandQueue(); }
	}
	void SelectVideoCard();
	void CreateDebugLayer();
	void CreateDXGIFactory();
	void CreateDevice();
	void CreateFence();
	void CheckIfSupport4xMSAA();
	void CreateCmdQueueAllocList();
	void CreateSwapChain();
	void CreateRTVAndDescriptorHeaps();
	void CreateRTVForBackBuffers();
	void CreateDSVForBackBuffer();
	void CreateAndSetViewPort();
	void CreateAndSetScissorRects();
	void FlushCommandQueue();
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
	ID3D12Resource* CurrentBackBuffer()const;
	void Draw();

	UINT mRTVDescriptorSize;
	UINT mDSVDescriptorSize;
	UINT mCBVUAVDescriptorSize;
	UINT CurrentBackBufferIndex = 0;
	static constexpr UINT mBackBufferCount = 2;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	bool IsSupport4xMSAA;
	UINT m4xMSAAQuality;
	UINT CurrentFence = 0;

	HWND hWnd;
	int Width;
	int Height;

	ComPtr<IDXGIFactory5> pIDXGIFactory5;
	ComPtr<IDXGIAdapter1> pIAdapter;
	ComPtr<ID3D12Device5> pID3DDevice;

	ComPtr<ID3D12CommandQueue> pCommandQueue;
	ComPtr<ID3D12CommandAllocator> pDirectCmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList> pCommandList;

	ComPtr<ID3D12DescriptorHeap> mRTVHeap;
	ComPtr<ID3D12DescriptorHeap> mDSVHeap;

	ComPtr<ID3D12Fence> pIFence;
	D3D12_VIEWPORT pViewPort;
	RECT pRECT;

	ComPtr<ID3D12Resource> mSwapChainBuffer[mBackBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

	ComPtr<IDXGISwapChain> pIDXGISwapChain;
#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> pDebugController;
#endif
};
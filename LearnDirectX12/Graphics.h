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

#define CALC_CBUFFER_BYTE_SIZE( byteSize ) ( (byteSize + 255) & ~255 )

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
	ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData,UINT64 byteSize,ComPtr<ID3D12Resource>& uploadBuffer);

	static ComPtr<ID3DBlob> CompileShader(const std::wstring& filename,const D3D_SHADER_MACRO* defines,const std::string& entrypoint,const std::string& target);

	///////////////////////////////////画空白画布////////////////////////////////////
	void DrawEmpty();
	////////////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////画方块用到的东西//////////////////////////////
	void DrawBox();
	void InitBox();
	void QuitBox();
	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};
	struct ObjectConstants {
		XMFLOAT4X4 WorldViewProj = XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	};
	ComPtr<ID3D12Resource> VertexBufferGpu = nullptr;
	ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> UploadCBuffer = nullptr;
	ComPtr<ID3D12RootSignature> RootSignature = nullptr;
	ComPtr<ID3D12PipelineState> PipelineState = nullptr;

	D3D12_INPUT_ELEMENT_DESC InputLayout[2];
	BYTE* CBufferDataPtr;
	ComPtr<ID3DBlob> ByteCodeVS;
	ComPtr<ID3DBlob> ByteCodePS;
	ComPtr<ID3D12DescriptorHeap> CBufferViewHeap = nullptr;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;

	Vertex vertices[8] = {
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) },
	{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) },
	{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) },
	{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) },
	{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) },
	{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) },
	{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) },
	{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) }
	};
	std::uint16_t indices[36] = {
		// 立方体前表面
		0, 1, 2,
		0, 2, 3,
		// 立方体后表面
		4, 6, 5,
		4, 7, 6,
		// 立方体左表面
		4, 5, 1,
		4, 1, 0,
		// 立方体右表面
		3, 2, 6,
		3, 6, 7,
		// 立方体上表面
		1, 5, 6,
		1, 6, 2,
		// 立方体下表面
		4, 0, 3,
		4, 3, 7
	};
	///////////////////////////////////////////////////////////////////////////////

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


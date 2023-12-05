#pragma once
#include <DirectXColors.h>
#include <dxgi.h>
#include "Camera.h"
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
	D3D12_CPU_DESCRIPTOR_HANDLE FetchIndexedViewCpuHandleFromCBV_SRV_UAVHeap(UINT Index);
	D3D12_GPU_DESCRIPTOR_HANDLE FetchIndexedViewGpuHandleFromCBV_SRV_UAVHeap(UINT Index);
	D3D12_CPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewCpuHandleFromRTVHeap(UINT Index);
	D3D12_GPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewGpuHandleFromRTVHeap(UINT Index);
	D3D12_CPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewCpuHandleFromDSVHeap(UINT Index);
	D3D12_GPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewGpuHandleFromDSVHeap(UINT Index);

	static D3D12_SHADER_BYTECODE GetShaderByteCodeFromBlob(ID3DBlob* bytecode);
	static XMFLOAT4X4 MatrixToFloat4x4(XMMATRIX& mat);
	static XMFLOAT4X4 MatrixToFloat4x4(XMMATRIX&& mat);
	static XMFLOAT4X4 RotateMatrixX(double angle);

	UINT AddViewOnCBVHeap(D3D12_CONSTANT_BUFFER_VIEW_DESC* desc);
	UINT AddViewOnSRVHeap(D3D12_SHADER_RESOURCE_VIEW_DESC* desc, ID3D12Resource* resource);
	UINT AddViewOnRTVHeap(D3D12_RENDER_TARGET_VIEW_DESC* desc, ID3D12Resource* resource);
	UINT AddViewOnDSVHeap(D3D12_DEPTH_STENCIL_VIEW_DESC* desc, ID3D12Resource* resource);
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
	ID3D12Resource* CurrentBackBuffer()const;
	ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData,UINT64 byteSize,ComPtr<ID3D12Resource>& uploadBuffer);
	ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData, UINT64 byteSize, ID3D12Resource* uploadBuffer);
	static ID3DBlob* CompileShader(const std::wstring& filename,const D3D_SHADER_MACRO* defines,const std::string& entrypoint,const std::string& target);

	UINT mRTVDescriptorSize;
	UINT mDSVDescriptorSize;
	UINT mCBVUAVDescriptorSize;
	UINT CurrentBackBufferIndex = 0;
	static constexpr UINT mBackBufferCount = 2;
	static constexpr DXGI_FORMAT DefaultDSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	static constexpr DXGI_FORMAT DefaultRTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	XMMATRIX Matrix4x4Identity = XMMATRIX(1,0,0,0	,0,1,0,0	,0,0,1,0	,0,0,0,1);
	XMFLOAT4X4 Float4x4Identity = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	bool IsSupport4xMSAA;
	UINT m4xMSAAQuality;
	UINT64 CurrentFence = 0;

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
	ComPtr<ID3D12DescriptorHeap> CBV_SRV_UAVHeap;
	UINT CBV_SRV_UAVHeap_StackPtr = 0;
	UINT RTVHeap_StackPtr = 0;
	UINT DSVHeap_StackPtr = 0;

	ComPtr<ID3D12Fence> pIFence;
	D3D12_VIEWPORT pViewPort;
	RECT pRECT;

	ComPtr<ID3D12Resource> mSwapChainBuffer[mBackBufferCount];
	UINT mSwapChainBufferIndex[mBackBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;
	UINT mDepthStencilBufferIndex;

	ComPtr<IDXGISwapChain> pIDXGISwapChain;
#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> pDebugController;
#endif
};


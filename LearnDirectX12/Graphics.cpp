#include "Graphics.h"

void Graphics::CreateDebugLayer() {
#if defined _DEBUG
	THROW_IF_ERROR(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)));
	pDebugController->EnableDebugLayer();
#endif // (DEBUG) || (_DEBUG)
}

void Graphics::CreateDXGIFactory() {
#if defined _DEBUG
	THROW_IF_ERROR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&pIDXGIFactory5)));
#else
	THROW_IF_ERROR(CreateDXGIFactory1(IID_PPV_ARGS(&pIDXGIFactory5)));
#endif
}

void Graphics::CreateDevice() {
	THROW_IF_ERROR(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&pID3DDevice)));
}

void Graphics::CreateFence() {
	THROW_IF_ERROR(pID3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pIFence)));
}

void Graphics::CheckIfSupport4xMSAA() {
	//检查是否支持4xmsaa
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = DefaultRTVFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	THROW_IF_ERROR(this->pID3DDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
	this->IsSupport4xMSAA = msQualityLevels.NumQualityLevels > 0;
}

void Graphics::CreateCmdQueueAllocList() {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	THROW_IF_ERROR(this->pID3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue)));
	THROW_IF_ERROR(this->pID3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(this->pDirectCmdListAlloc.GetAddressOf())));
	THROW_IF_ERROR(this->pID3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pDirectCmdListAlloc.Get(), nullptr, IID_PPV_ARGS(this->pCommandList.GetAddressOf())));
	THROW_IF_ERROR(pCommandList->Close());
	THROW_IF_ERROR(pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(pCommandList->Reset(pDirectCmdListAlloc.Get(), nullptr));
}

void Graphics::CreateSwapChain() {
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = this->Width;
	sd.BufferDesc.Height = this->Height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DefaultRTVFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = mBackBufferCount;
	sd.OutputWindow = this->hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	THROW_IF_ERROR(this->pIDXGIFactory5->CreateSwapChain(this->pCommandQueue.Get(), &sd, pIDXGISwapChain.GetAddressOf()));
	THROW_IF_ERROR(pIDXGIFactory5->MakeWindowAssociation(this->hWnd, DXGI_MWA_NO_ALT_ENTER));
}

void Graphics::CreateRTVAndDescriptorHeaps() {
	//为交换链的缓冲区创建渲染目标视图和深度模板视图
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = mBackBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	THROW_IF_ERROR(pID3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	THROW_IF_ERROR(pID3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDSVHeap)));
}

void Graphics::CreateRTVForBackBuffers() {
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < mBackBufferCount; i++) {
		THROW_IF_ERROR(pIDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		pID3DDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRTVDescriptorSize);
	}
}

void Graphics::CreateDSVForBackBuffer() {
	D3D12_RESOURCE_DESC depthstnecilDesc;
	depthstnecilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthstnecilDesc.Alignment = 0;
	depthstnecilDesc.Height = this->Height;
	depthstnecilDesc.Width = this->Width;
	depthstnecilDesc.DepthOrArraySize = 1;
	depthstnecilDesc.MipLevels = 1;
	depthstnecilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthstnecilDesc.SampleDesc.Count = 1;
	depthstnecilDesc.SampleDesc.Quality = 0;
	depthstnecilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthstnecilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	THROW_IF_ERROR(pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthstnecilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));
	pID3DDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, DepthStencilView());
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void Graphics::CreateAndSetViewPort() {
	this->pViewPort.TopLeftX = 0;
	this->pViewPort.TopLeftY = 0;
	this->pViewPort.Width = this->Width;
	this->pViewPort.Height = this->Height;
	this->pViewPort.MinDepth = 0;
	this->pViewPort.MaxDepth = 1;
	pCommandList->RSSetViewports(1, &pViewPort);
}

void Graphics::CreateAndSetScissorRects() {
	pRECT.left = 0;
	pRECT.right = this->Width;
	pRECT.top = 0;
	pRECT.bottom = this->Height;
	pCommandList->RSSetScissorRects(1, &pRECT);
}

void Graphics::FlushCommandQueue() {
	CurrentFence++;
	THROW_IF_ERROR(pCommandQueue->Signal(pIFence.Get(), CurrentFence));
	if (pIFence->GetCompletedValue() < CurrentFence) {
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
		THROW_IF_ERROR(pIFence->SetEventOnCompletion(CurrentFence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
D3D12_CPU_DESCRIPTOR_HANDLE Graphics::CurrentBackBufferView() const {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), this->CurrentBackBufferIndex, this->mRTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewCpuHandleFromCBV_SRV_UAVHeap(UINT Index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(CBV_SRV_UAVHeap->GetCPUDescriptorHandleForHeapStart(), Index, this->mCBVUAVDescriptorSize);
}
D3D12_GPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewGpuHandleFromCBV_SRV_UAVHeap(UINT Index)
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(CBV_SRV_UAVHeap->GetGPUDescriptorHandleForHeapStart(), Index, this->mCBVUAVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewCpuHandleFromRTVHeap(UINT Index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), Index, this->mRTVDescriptorSize);
}
D3D12_GPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewGpuHandleFromRTVHeap(UINT Index)
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(mRTVHeap->GetGPUDescriptorHandleForHeapStart(), Index, this->mRTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewCpuHandleFromDSVHeap(UINT Index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mDSVHeap->GetCPUDescriptorHandleForHeapStart(), Index, this->mDSVDescriptorSize);
}
D3D12_GPU_DESCRIPTOR_HANDLE Graphics::FetchIndexedViewGpuHandleFromDSVHeap(UINT Index)
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(mDSVHeap->GetGPUDescriptorHandleForHeapStart(), Index, this->mDSVDescriptorSize);
}

D3D12_SHADER_BYTECODE Graphics::GetShaderByteCodeFromBlob(ID3DBlob* bytecode)
{
	D3D12_SHADER_BYTECODE tmp = { bytecode->GetBufferPointer(),bytecode->GetBufferSize() };
	return tmp;
}

XMFLOAT4X4 Graphics::MatrixToFloat4x4(XMMATRIX& mat)
{
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, mat);
	return temp;
}
XMFLOAT4X4 Graphics::MatrixToFloat4x4(XMMATRIX&& mat)
{
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, mat);
	return temp;
}

XMFLOAT4X4 Graphics::RotateMatrixX(double angle)
{
	return XMFLOAT4X4(1,0,0,0,	0,cos(angle),-sin(angle),0,	0,sin(angle),cos(angle),0	,0,0,0,1);
}

UINT Graphics::AddViewOnCBVHeap(D3D12_CONSTANT_BUFFER_VIEW_DESC* desc)
{
	pID3DDevice->CreateConstantBufferView(desc, CD3DX12_CPU_DESCRIPTOR_HANDLE(CBV_SRV_UAVHeap->GetCPUDescriptorHandleForHeapStart(), CBV_SRV_UAVHeap_StackPtr, this->mCBVUAVDescriptorSize));
	CBV_SRV_UAVHeap_StackPtr++;
	return CBV_SRV_UAVHeap_StackPtr - 1;
}
UINT Graphics::AddViewOnSRVHeap(D3D12_SHADER_RESOURCE_VIEW_DESC* desc,ID3D12Resource* resource)
{
	pID3DDevice->CreateShaderResourceView(resource, desc, CD3DX12_CPU_DESCRIPTOR_HANDLE(CBV_SRV_UAVHeap->GetCPUDescriptorHandleForHeapStart(), CBV_SRV_UAVHeap_StackPtr, this->mCBVUAVDescriptorSize));
	CBV_SRV_UAVHeap_StackPtr++;
	return CBV_SRV_UAVHeap_StackPtr - 1;
}
UINT Graphics::AddViewOnRTVHeap(D3D12_RENDER_TARGET_VIEW_DESC* desc, ID3D12Resource* resource)
{
	pID3DDevice->CreateRenderTargetView(resource, desc, CD3DX12_CPU_DESCRIPTOR_HANDLE(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), RTVHeap_StackPtr, this->mRTVDescriptorSize));
	RTVHeap_StackPtr++;
	return RTVHeap_StackPtr - 1;
}
UINT Graphics::AddViewOnDSVHeap(D3D12_DEPTH_STENCIL_VIEW_DESC* desc, ID3D12Resource* resource)
{
	pID3DDevice->CreateDepthStencilView(resource, desc, CD3DX12_CPU_DESCRIPTOR_HANDLE(mDSVHeap->GetCPUDescriptorHandleForHeapStart(), DSVHeap_StackPtr, this->mDSVDescriptorSize));
	DSVHeap_StackPtr++;
	return DSVHeap_StackPtr - 1;
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphics::DepthStencilView() const {
	return mDSVHeap->GetCPUDescriptorHandleForHeapStart();
}
ID3D12Resource* Graphics::CurrentBackBuffer()const
{
	return mSwapChainBuffer[CurrentBackBufferIndex].Get();
}

ComPtr<ID3D12Resource> Graphics::CreateDefaultBuffer(const void* initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer)
{
	ComPtr<ID3D12Resource> defaultBuffer;
	THROW_IF_ERROR(pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize), D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(defaultBuffer.GetAddressOf())));
	THROW_IF_ERROR(pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf())));
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(pCommandList.Get(), defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	return defaultBuffer;
}
ComPtr<ID3D12Resource> Graphics::CreateDefaultBuffer(const void* initData, UINT64 byteSize, ID3D12Resource* uploadBuffer)
{
	ComPtr<ID3D12Resource> defaultBuffer;
	THROW_IF_ERROR(pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize), D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(defaultBuffer.GetAddressOf())));
	THROW_IF_ERROR(pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer)));
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(pCommandList.Get(), defaultBuffer.Get(), uploadBuffer, 0, 0, 1, &subResourceData);

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	//提交命令，清空队列
	THROW_IF_ERROR(pCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { pCommandList.Get() };
	pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();
	THROW_IF_ERROR(pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(pCommandList->Reset(pDirectCmdListAlloc.Get(), nullptr));
	uploadBuffer->Release();
	return defaultBuffer;
}

ID3DBlob* Graphics::CompileShader(bool* IsAnyError, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) |defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors = nullptr;
	HRESULT hr=D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);
	if (errors != nullptr) {
		const char* error_text = (char*)(errors->GetBufferPointer());
		this->StatusConsole->PrintLineA(error_text);
		*IsAnyError = true;
	}
	return byteCode;
}

Graphics::Graphics(MyWindow* Wnd, Console* StatusConsole) {
	this->hWnd = Wnd->hWnd;
	this->Height = Wnd->Height;
	this->Width = Wnd->Width;
	this->StatusConsole = StatusConsole;
	CreateDebugLayer();
	CreateDXGIFactory();
	CreateDevice();
	CreateFence();
	mRTVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDSVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCBVUAVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CheckIfSupport4xMSAA();
	CreateCmdQueueAllocList();
	CreateSwapChain();
	CreateAndSetViewPort();
	CreateAndSetScissorRects();
}

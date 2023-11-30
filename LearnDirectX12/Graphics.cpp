#include "Graphics.h"

void Graphics::SelectVideoCard() {
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(adapterIndex, &pIAdapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc = {};
		pIAdapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{//软件虚拟适配器，跳过
			continue;
		}
		//检查适配器对D3D支持的兼容级别，这里直接要求支持12.1的能力，注意返回接口的那个参数被置为了nullptr，这样
		//就不会实际创建一个设备了，也不用我们啰嗦的再调用release来释放接口。这也是一个重要的技巧，请记住！
		if (SUCCEEDED(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_2, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
		if (desc.VendorId == 0x10DE) { // 替换为目标显卡的厂商ID
			break;
		}
	}
}

void Graphics::CreateDebugLayer() {
	THROW_IF_ERROR(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)));
	pDebugController->EnableDebugLayer();
}

void Graphics::CreateDXGIFactory() {
#if defined _DEBUG
	THROW_IF_ERROR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,IID_PPV_ARGS(&pIDXGIFactory5)));
#else
	THROW_IF_ERROR(CreateDXGIFactory1(IID_PPV_ARGS(&pIDXGIFactory5)));
#endif
	THROW_IF_ERROR(pIDXGIFactory5->MakeWindowAssociation(this->hWnd, DXGI_MWA_NO_ALT_ENTER));
}

void Graphics::CreateDevice() {
	THROW_IF_ERROR(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&pID3DDevice)));
}

void Graphics::CreateFence() {
	THROW_IF_ERROR(pID3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pIFence)));
}

void Graphics::CheckIfSupport4xMSAA() {
	//检查是否支持4xmsaa
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
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
}

void Graphics::CreateSwapChain() {
	//if (pIDXGISwapChain != nullptr) { pIDXGISwapChain.Reset(); }
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = this->Width;
	sd.BufferDesc.Height = this->Height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
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
	D3D12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	THROW_IF_ERROR(pID3DDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &depthstnecilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));
	pID3DDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, DepthStencilView());
	CD3DX12_RESOURCE_BARRIER rb = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	pCommandList->ResourceBarrier(1, &rb);
}

void Graphics::CreateAndSetViewPort() {
	this->pViewPort.TopLeftX = 0;
	this->pViewPort.TopLeftY = 0;
	this->pViewPort.Width = this->Width;
	this->pViewPort.Height = this->Height;
	this->pViewPort.MinDepth = 0;
	this->pViewPort.MaxDepth = 1;
}

void Graphics::CreateAndSetScissorRects() {
	pRECT.left = 0;
	pRECT.right = this->Width;
	pRECT.top = 0;
	pRECT.bottom = this->Height;
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

D3D12_CPU_DESCRIPTOR_HANDLE Graphics::DepthStencilView() const {
	return mDSVHeap->GetCPUDescriptorHandleForHeapStart();
}
ID3D12Resource* Graphics::CurrentBackBuffer()const
{
	return mSwapChainBuffer[CurrentBackBufferIndex].Get();
}

void Graphics::Draw() {
	THROW_IF_ERROR(pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(pCommandList->Reset(pDirectCmdListAlloc.Get(), nullptr));

	CD3DX12_RESOURCE_BARRIER rb = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	pCommandList->ResourceBarrier(1, &rb);

	pCommandList->RSSetScissorRects(1, &pRECT);
	pCommandList->RSSetViewports(1, &pViewPort);

	pCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	pCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	auto cbv = CurrentBackBufferView();
	auto dsv = DepthStencilView();
	pCommandList->OMSetRenderTargets(1, &cbv, true, &dsv);

	rb = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	pCommandList->ResourceBarrier(1, &rb);

	THROW_IF_ERROR(pCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { pCommandList.Get() };
	pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	THROW_IF_ERROR(pIDXGISwapChain->Present(0, 0));
	CurrentBackBufferIndex = (CurrentBackBufferIndex + 1) % mBackBufferCount;
	FlushCommandQueue();
}

Graphics::Graphics(MyWindow* Wnd) {
	this->hWnd = Wnd->hWnd;
	this->Height = Wnd->Height;
	this->Width = Wnd->Width;
	
	CreateDebugLayer();
	CreateDXGIFactory();
	SelectVideoCard();
	CreateDevice();
	CreateFence();
	mRTVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDSVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCBVUAVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CheckIfSupport4xMSAA();
	CreateCmdQueueAllocList();
	CreateSwapChain();
	CreateRTVAndDescriptorHeaps();
	CreateRTVForBackBuffers();
	CreateDSVForBackBuffer();
	CreateAndSetViewPort();
	CreateAndSetScissorRects();
}


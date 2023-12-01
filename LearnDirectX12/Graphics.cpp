#include "Graphics.h"

void Graphics::CreateDebugLayer() {
#ifdef DEBUG || _DEBUG
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
	THROW_IF_ERROR(pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(pCommandList->Reset(pDirectCmdListAlloc.Get(), nullptr));
}

void Graphics::CreateSwapChain() {
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

ComPtr<ID3DBlob> Graphics::CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) |defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	THROW_IF_ERROR(D3DCompileFromFile(filename.c_str(), defines,D3D_COMPILE_STANDARD_FILE_INCLUDE,entrypoint.c_str(), target.c_str(),compileFlags,0,&byteCode, &errors));
	if (errors != nullptr){ OutputDebugStringA((char*)errors->GetBufferPointer()); }
	return byteCode;
}

Graphics::Graphics(MyWindow* Wnd) {
	this->hWnd = Wnd->hWnd;
	this->Height = Wnd->Height;
	this->Width = Wnd->Width;

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
	CreateRTVAndDescriptorHeaps();
	CreateRTVForBackBuffers();
	CreateDSVForBackBuffer();
	CreateAndSetViewPort();
	CreateAndSetScissorRects();
	THROW_IF_ERROR(pCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { pCommandList.Get() };
	pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();
}

void Graphics::InitBox()
{
	THROW_IF_ERROR(pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(pCommandList->Reset(pDirectCmdListAlloc.Get(), nullptr));
	//创建常量缓冲的描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	THROW_IF_ERROR(pID3DDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(CBufferViewHeap.GetAddressOf())));

	//创建常量缓冲
	UINT CBufferByteSize = CALC_CBUFFER_BYTE_SIZE(sizeof(ObjectConstants));
	THROW_IF_ERROR(pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(CBufferByteSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(UploadCBuffer.GetAddressOf())));
	//将常量缓冲的内容和CBufferDataPtr绑定
	THROW_IF_ERROR(UploadCBuffer->Map(0, nullptr, reinterpret_cast<void**>(&CBufferDataPtr)));
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = UploadCBuffer->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = CBufferByteSize;
	pID3DDevice->CreateConstantBufferView(&cbvDesc, CBufferViewHeap->GetCPUDescriptorHandleForHeapStart());

	//创建根签名中的参数
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];
	//给常量缓冲创建一个描述表
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
	//创建根签名
	CD3DX12_ROOT_SIGNATURE_DESC rootsigDesc = CD3DX12_ROOT_SIGNATURE_DESC(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3D10Blob> serializedRootSig = nullptr;
	ComPtr<ID3D10Blob> errorBlob = nullptr;
	THROW_IF_ERROR(D3D12SerializeRootSignature(&rootsigDesc,D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf()));
	if (errorBlob != nullptr){ OutputDebugStringA((char*)errorBlob->GetBufferPointer()); }
	THROW_IF_ERROR(pID3DDevice->CreateRootSignature(0,serializedRootSig.Get()->GetBufferPointer(), serializedRootSig.Get()->GetBufferSize(), IID_PPV_ARGS(RootSignature.GetAddressOf())));

	//设置几何体
	//创建顶点缓冲视图
	const UINT64 vbByteSize = 8 * sizeof(Vertex);
	VertexBufferGpu = CreateDefaultBuffer(vertices, vbByteSize, VertexBufferUploader);
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = VertexBufferGpu->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(Vertex);
	vbv.SizeInBytes = vbByteSize;
	VertexBufferView = vbv;
	//创建索引缓冲和视图
	const UINT ibByteSize = 36 * sizeof(std::uint16_t);
	IndexBufferGPU = CreateDefaultBuffer(indices, ibByteSize, IndexBufferUploader);
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = DXGI_FORMAT_R16_UINT;
	ibv.SizeInBytes = ibByteSize;
	IndexBufferView = ibv;

	//编译着色器和顶点输入布局
	ByteCodeVS = CompileShader(L"D:\\LearnDirectX12\\LearnDirectX12\\Shaders\\DrawCube.hlsl",nullptr,"VS","vs_5_0");
	ByteCodePS = CompileShader(L"D:\\LearnDirectX12\\LearnDirectX12\\Shaders\\DrawCube.hlsl", nullptr, "PS", "ps_5_0");
	InputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputLayout[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	//设置管线状态
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { InputLayout ,2};
	psoDesc.pRootSignature = RootSignature.Get();
	psoDesc.VS = { reinterpret_cast<BYTE*>(ByteCodeVS.Get()->GetBufferPointer()),ByteCodeVS.Get()->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(ByteCodePS.Get()->GetBufferPointer()),ByteCodePS.Get()->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	THROW_IF_ERROR(pID3DDevice->CreateGraphicsPipelineState(&psoDesc,IID_PPV_ARGS(&PipelineState)));

	//将上述操作提交执行
	THROW_IF_ERROR(pCommandList->Close());
	ID3D12CommandList* cmdLists[] = {pCommandList.Get()};
	pCommandQueue->ExecuteCommandLists(_countof(cmdLists),cmdLists);
	FlushCommandQueue();
}
void Graphics::QuitBox()
{
	UploadCBuffer->Unmap(0,nullptr);
}
void Graphics::DrawEmpty() {
	THROW_IF_ERROR(pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(pCommandList->Reset(pDirectCmdListAlloc.Get(), nullptr));

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	pCommandList->RSSetScissorRects(1, &pRECT);
	pCommandList->RSSetViewports(1, &pViewPort);

	pCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	pCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	auto cbv = CurrentBackBufferView();
	auto dsv = DepthStencilView();
	pCommandList->OMSetRenderTargets(1, &cbv, true, &dsv);

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));

	THROW_IF_ERROR(pCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { pCommandList.Get() };
	pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	THROW_IF_ERROR(pIDXGISwapChain->Present(0, 0));
	CurrentBackBufferIndex = (CurrentBackBufferIndex + 1) % mBackBufferCount;
	FlushCommandQueue();
}

void Graphics::DrawBox()
{
	THROW_IF_ERROR(pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(pCommandList->Reset(pDirectCmdListAlloc.Get(), PipelineState.Get()));
	//设置裁剪矩形和视口
	pCommandList->RSSetScissorRects(1, &pRECT);
	pCommandList->RSSetViewports(1, &pViewPort);
	
	//把后缓冲设置为渲染目标
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	//清空渲染目标和深度模板缓冲
	pCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	pCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	//设置渲染目标
	pCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	//设置常量缓冲描述符堆
	ID3D12DescriptorHeap* descriptorHeaps[] = {CBufferViewHeap.Get()};
	pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//设置根签名
	pCommandList->SetGraphicsRootSignature(RootSignature.Get());
	//绑定顶点和索引缓冲
	pCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetIndexBuffer(&IndexBufferView);
	//设置根描述符表
	pCommandList->SetGraphicsRootDescriptorTable(0,CBufferViewHeap->GetGPUDescriptorHandleForHeapStart());
	//绘制
	pCommandList->DrawIndexedInstanced(36,1,0,0,0);
	//后缓冲转换为呈现状态
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	//提交命令，翻转交换链，清空队列
	THROW_IF_ERROR(pCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { pCommandList.Get() };
	pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	THROW_IF_ERROR(pIDXGISwapChain->Present(0, 0));
	CurrentBackBufferIndex = (CurrentBackBufferIndex + 1) % mBackBufferCount;
	FlushCommandQueue();
}

void Graphics::Update(Camera* camera){
	ObjectConstants obj;
	XMStoreFloat4x4(&obj.WorldViewProj, XMMatrixTranspose(camera->CameraVPMatrix));
	memcpy(CBufferDataPtr,&obj,sizeof(ObjectConstants));
}

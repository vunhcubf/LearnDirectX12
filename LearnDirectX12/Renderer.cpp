#include "Renderer.h"

Renderer::Renderer(Graphics* graphics)
{
	this->graphics = graphics;
	Initialize();
	//初始化需要用到的全屏网格
	//设置几何体
	//创建顶点缓冲视图
	std::vector<Mesh::Vertex_POS_COLOR_UV1_UV2_Normal_Tangent> vertices(Mesh::FullScreenMesh, Mesh::FullScreenMesh+4*sizeof(Mesh::Vertex_POS_COLOR_UV1_UV2_Normal_Tangent));
	std::vector<UINT> indices(Mesh::FullScreenMeshIndices, Mesh::FullScreenMeshIndices+6*sizeof(UINT));
	FullScreenMesh = new Mesh(graphics,vertices,indices,4,6,&Mesh::CBufferPerObject(XMFLOAT4X4(1,0,0,0 ,0,1,0,0 ,0,0,1,0 ,0,0,0,1),XMFLOAT4(1,0,0,0)));
	BlitVS = graphics->CompileShader(nullptr, L"D:\\LearnDirectX12\\LearnDirectX12\\Shaders\\Blit.hlsl", nullptr, "VS", "vs_5_0");
	BlitPS = graphics->CompileShader(nullptr, L"D:\\LearnDirectX12\\LearnDirectX12\\Shaders\\Blit.hlsl", nullptr, "PS", "ps_5_0");
	MaterialResources resources1;
	resources1.AddFloat4(XMFLOAT4(0.5, 0.5, 0.5, 0.5));
	D3D12_RASTERIZER_DESC RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	D3D12_DEPTH_STENCIL_DESC DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	BlitMaterial = new Material(Graphics::GetShaderByteCodeFromBlob(BlitVS.Get()), Graphics::GetShaderByteCodeFromBlob(BlitPS.Get()),graphics, resources1,1);
	BlitMaterial->ResetDepthStencilState(DepthStencilState);
	BlitMaterial->ResetRasterizerState(RasterizerState);
}

void Renderer::Blit(Texture* src, Texture* dest)
{
	BlitMaterial->SetTexture(src);
	BlitMaterial->ResetRTVFormat(src->TextureFormat);
	SetRenderTarget(dest->TextureRTVIndex, graphics->GetSwapchainDSBufferIndex(), dest->TextueResource.Get(),graphics->GetSwapchainDSBuffer());
	DrawMesh(BlitMaterial, FullScreenMesh);
	ExcuteCommandList();
	ClearCommandList();
}

void Renderer::Blit(Texture* src)
{
	BlitMaterial->SetTexture(src);
	BlitMaterial->ResetRTVFormat(src->TextureFormat);
	SetRenderTarget(graphics->GetSwapchainRTVIndex(), graphics->GetSwapchainDSBufferIndex(), graphics->GetSwapchainColorBuffer(), graphics->GetSwapchainDSBuffer());
	DrawMesh(BlitMaterial, FullScreenMesh);
	ExcuteCommandList();
	ClearCommandList();
}

void Renderer::SetRenderTarget(UINT CameraColorTargetIndex, UINT CameraDepthStencilTargetIndex, ID3D12Resource* ColorResource, ID3D12Resource* DepthStencilResource)
{
	this->CameraColorTargetIndex = CameraColorTargetIndex;
	this->CameraDepthStencilTargetIndex = CameraDepthStencilTargetIndex;
	this->ColorResource = ColorResource;
	this->DepthStencilResource = DepthStencilResource;
}
void Renderer::SetRenderTarget(Texture* dest,UINT CameraDepthStencilTargetIndex, ID3D12Resource* DepthStencilResource)
{
	this->CameraColorTargetIndex = dest->TextureRTVIndex;
	this->CameraDepthStencilTargetIndex = CameraDepthStencilTargetIndex;
	this->ColorResource = dest->TextueResource.Get();
	this->DepthStencilResource = DepthStencilResource;
}

void Renderer::SetRenderTarget(std::vector<Texture*> dest, UINT NumRenderTarget, UINT CameraDepthStencilTargetIndex, ID3D12Resource* DepthStencilResource)
{
	this->Rendertargets = dest;
	this->NumRenderTarget = NumRenderTarget;
	this->DepthStencilResource = DepthStencilResource;
	this->CameraDepthStencilTargetIndex = CameraDepthStencilTargetIndex;
}

void Renderer::ClearRenderTarget()
{
	WillClearRenderTarget = true;
}

void Renderer::DrawMesh(Material* mat, Mesh* mesh)
{
	mat->RefreshMaterial(graphics);
	//将渲染目标设置为目标状态
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	if (WillClearRenderTarget) {
		//清空渲染目标和深度模板缓冲
		graphics->pCommandList->ClearRenderTargetView(graphics->FetchIndexedViewCpuHandleFromRTVHeap(CameraColorTargetIndex), Colors::LightSteelBlue, 0, nullptr);
		graphics->pCommandList->ClearDepthStencilView(graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}
	//设置渲染目标
	graphics->pCommandList->OMSetRenderTargets(1, &graphics->FetchIndexedViewCpuHandleFromRTVHeap(CameraColorTargetIndex), true, &graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex));
	//设置常量缓冲描述符堆
	ID3D12DescriptorHeap* descriptorHeaps[] = { graphics->CBV_SRV_UAVHeap.Get() };
	graphics->pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//设置根签名
	graphics->pCommandList->SetGraphicsRootSignature(mat->GetRootSignaturePtr());
	//绑定顶点和索引缓冲
	graphics->pCommandList->IASetVertexBuffers(0, 1, mesh->GetVertexBufferViewPtr());
	graphics->pCommandList->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());
	graphics->pCommandList->IASetIndexBuffer(mesh->GetIndexBufferViewPtr());
	//设置根签名对应的资源
	mat->BindRootSignatureResource(graphics);
	graphics->pCommandList->SetPipelineState(mat->GetPipelineStatePtr());
	//每次渲染之前都要手动设置视口
	graphics->pCommandList->RSSetScissorRects(1, &graphics->pRECT);
	graphics->pCommandList->RSSetViewports(1, &graphics->pViewPort);
	//绘制
	graphics->pCommandList->DrawIndexedInstanced(mesh->GetIndicesCount(), 1, 0, 0, 0);
	//渲染目标转换为呈现状态
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	WillClearRenderTarget = false;
}
void Renderer::DrawRenderer(std::unordered_map<std::wstring, Object*> objs)
{
	//将渲染目标设置为目标状态
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	//清空渲染目标和深度模板缓冲
	graphics->pCommandList->ClearRenderTargetView(graphics->FetchIndexedViewCpuHandleFromRTVHeap(CameraColorTargetIndex), Colors::LightSteelBlue, 0, nullptr);
	graphics->pCommandList->ClearDepthStencilView(graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	for (const auto& obj:objs) {
		for (const auto& it : obj.second->material->TextureResources) {
			obj.second->material->SetTexture(it);
		}
		obj.second->material->SetConstantBuffer(obj.second->mesh->GetCBufferPerObjPtr());
		obj.second->material->SetConstantBuffer(obj.second->cbufferPerFrame);
		obj.second->material->SetConstantBuffer(obj.second->cbufferPerMaterial);
		obj.second->material->RefreshMaterial(graphics);
		
		//设置渲染目标
		graphics->pCommandList->OMSetRenderTargets(1, &graphics->FetchIndexedViewCpuHandleFromRTVHeap(CameraColorTargetIndex), true, &graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex));
		//设置常量缓冲描述符堆
		ID3D12DescriptorHeap* descriptorHeaps[] = { graphics->CBV_SRV_UAVHeap.Get() };
		graphics->pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		//设置根签名
		graphics->pCommandList->SetGraphicsRootSignature(obj.second->material->GetRootSignaturePtr());
		//绑定顶点和索引缓冲
		graphics->pCommandList->IASetVertexBuffers(0, 1, obj.second->mesh->GetVertexBufferViewPtr());
		graphics->pCommandList->IASetPrimitiveTopology(obj.second->mesh->GetPrimitiveTopology());
		graphics->pCommandList->IASetIndexBuffer(obj.second->mesh->GetIndexBufferViewPtr());
		//设置根签名对应的资源
		obj.second->material->BindRootSignatureResource(graphics);
		graphics->pCommandList->SetPipelineState(obj.second->material->GetPipelineStatePtr());
		//每次渲染之前都要手动设置视口
		graphics->pCommandList->RSSetScissorRects(1, &graphics->pRECT);
		graphics->pCommandList->RSSetViewports(1, &graphics->pViewPort);
		//绘制
		graphics->pCommandList->DrawIndexedInstanced(obj.second->mesh->GetIndicesCount(), 1, 0, 0, 0);
		//渲染目标转换为呈现状态
	}
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	ExcuteCommandList();
	ClearCommandList();
}
void Renderer::DrawRendererMrt(std::unordered_map<std::wstring, Object*> objs)
{
	if (Rendertargets.size()==0 ||NumRenderTarget<=1)throw std::runtime_error("multi render targets are not set");
	//将渲染目标设置为目标状态
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	//清空渲染目标和深度模板缓冲
	for (int i = 0; i < NumRenderTarget; i++) {
		const FLOAT clear_color[4] = { 0.5,0.5,0.5,0.5 };
		graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Rendertargets.at(i)->TextueResource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		graphics->pCommandList->ClearRenderTargetView(graphics->FetchIndexedViewCpuHandleFromRTVHeap(Rendertargets.at(i)->TextureRTVIndex), clear_color, 0, nullptr);
	}
	graphics->pCommandList->ClearDepthStencilView(graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	for (const auto& obj : objs) {
		for (const auto& it : obj.second->material->TextureResources) {
			obj.second->material->SetTexture(it);
		}
		obj.second->material->SetConstantBuffer(obj.second->mesh->GetCBufferPerObjPtr());
		obj.second->material->SetConstantBuffer(obj.second->cbufferPerFrame);
		obj.second->material->SetConstantBuffer(obj.second->cbufferPerMaterial);
		obj.second->material->ResetNumRenderTargets(this->NumRenderTarget);
		std::vector<DXGI_FORMAT> formats;
		for (int i = 0; i < this->NumRenderTarget; i++) {
			formats.push_back(Rendertargets.at(i)->TextureFormat);
		}
		obj.second->material->ResetRTVFormats(formats.data());
		obj.second->material->RefreshMaterial(graphics);
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtv_handles;
		//获取多目标的所有句柄
		for (int i = 0; i < NumRenderTarget; i++) {
			rtv_handles.push_back(graphics->FetchIndexedViewCpuHandleFromRTVHeap(Rendertargets.at(i)->TextureRTVIndex));
		}
		//设置渲染目标
		graphics->pCommandList->OMSetRenderTargets(NumRenderTarget, rtv_handles.data(), true, &graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex));
		//设置常量缓冲描述符堆
		ID3D12DescriptorHeap* descriptorHeaps[] = { graphics->CBV_SRV_UAVHeap.Get() };
		graphics->pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		//设置根签名
		graphics->pCommandList->SetGraphicsRootSignature(obj.second->material->GetRootSignaturePtr());
		//绑定顶点和索引缓冲
		graphics->pCommandList->IASetVertexBuffers(0, 1, obj.second->mesh->GetVertexBufferViewPtr());
		graphics->pCommandList->IASetPrimitiveTopology(obj.second->mesh->GetPrimitiveTopology());
		graphics->pCommandList->IASetIndexBuffer(obj.second->mesh->GetIndexBufferViewPtr());
		//设置根签名对应的资源
		obj.second->material->BindRootSignatureResource(graphics);
		graphics->pCommandList->SetPipelineState(obj.second->material->GetPipelineStatePtr());
		//每次渲染之前都要手动设置视口
		graphics->pCommandList->RSSetScissorRects(1, &graphics->pRECT);
		graphics->pCommandList->RSSetViewports(1, &graphics->pViewPort);
		//绘制
		graphics->pCommandList->DrawIndexedInstanced(obj.second->mesh->GetIndicesCount(), 1, 0, 0, 0);
		//渲染目标转换为呈现状态
		obj.second->material->ResetNumRenderTargets(1);
	}
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));
	for (int i = 0; i < NumRenderTarget; i++) {
		graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Rendertargets.at(i)->TextueResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}
	ExcuteCommandList();
	ClearCommandList();
	this->NumRenderTarget = 1;
	this->Rendertargets.clear();
}
void Renderer::WaitGpu()
{
	graphics->FlushCommandQueue();
}

void Renderer::ClearCommandList()
{
	THROW_IF_ERROR(graphics->pCommandList->Reset(graphics->pDirectCmdListAlloc.Get(), nullptr));
}

void Renderer::ClearCommandAllocater()
{
	THROW_IF_ERROR(graphics->pDirectCmdListAlloc->Reset());
}

void Renderer::RefreshSwapChain()
{
	THROW_IF_ERROR(graphics->pIDXGISwapChain->Present(0, 0));
	graphics->CurrentBackBufferIndex = (graphics->CurrentBackBufferIndex + 1) % graphics->mBackBufferCount;
}
void Renderer::ExcuteCommandList()
{
	THROW_IF_ERROR(graphics->pCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { graphics->pCommandList.Get() };
	graphics->pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void Renderer::ExcuteCommandListWithBlock()
{
	//提交命令，清空队列
	THROW_IF_ERROR(graphics->pCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { graphics->pCommandList.Get() };
	graphics->pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	graphics->FlushCommandQueue();
	THROW_IF_ERROR(graphics->pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(graphics->pCommandList->Reset(graphics->pDirectCmdListAlloc.Get(), nullptr));
}

void Renderer::ExcuteCommandListWithBlockWithResetSwapChain()
{
	//提交命令，清空队列
	THROW_IF_ERROR(graphics->pCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { graphics->pCommandList.Get() };
	graphics->pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	RefreshSwapChain();
	graphics->FlushCommandQueue();
	THROW_IF_ERROR(graphics->pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(graphics->pCommandList->Reset(graphics->pDirectCmdListAlloc.Get(), nullptr));
}

void Renderer::Initialize()
{
	//创建RTV描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = 64;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	THROW_IF_ERROR(graphics->pID3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(graphics->mRTVHeap.GetAddressOf())));
	//创建DSV描述符
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 64;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	THROW_IF_ERROR(graphics->pID3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(graphics->mDSVHeap.GetAddressOf())));
	//创建常量缓冲的描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 64;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	THROW_IF_ERROR(graphics->pID3DDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(graphics->CBV_SRV_UAVHeap.GetAddressOf())));

	//创建两张后缓冲视图
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(graphics->mRTVHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < graphics->mBackBufferCount; i++) {
		THROW_IF_ERROR(graphics->pIDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&(graphics->mSwapChainBuffer[i]))));
		graphics->mSwapChainBufferIndex[i] = graphics->AddViewOnRTVHeap(nullptr, graphics->mSwapChainBuffer[i].Get());
	}
	//创建后缓冲对应的深度模板视图和资源
	D3D12_RESOURCE_DESC depthstnecilDesc;
	depthstnecilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthstnecilDesc.Alignment = 0;
	depthstnecilDesc.Height = graphics->Height;
	depthstnecilDesc.Width = graphics->Width;
	depthstnecilDesc.DepthOrArraySize = 1;
	depthstnecilDesc.MipLevels = 1;
	depthstnecilDesc.Format = graphics->DefaultDSVFormat;
	depthstnecilDesc.SampleDesc.Count = 1;
	depthstnecilDesc.SampleDesc.Quality = 0;
	depthstnecilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthstnecilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = graphics->DefaultDSVFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	THROW_IF_ERROR(graphics->pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthstnecilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(graphics->mDepthStencilBuffer.GetAddressOf())));
	graphics->mDepthStencilBufferIndex = graphics->AddViewOnDSVHeap(nullptr, graphics->mDepthStencilBuffer.Get());
}

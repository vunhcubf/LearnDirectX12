#include "Renderer.h"

void Renderer::SetRenderTarget(UINT CameraColorTargetIndex, UINT CameraDepthStencilTargetIndex, ID3D12Resource* ColorResource, ID3D12Resource* DepthStencilResource)
{
	this->CameraColorTargetIndex = CameraColorTargetIndex;
	this->CameraDepthStencilTargetIndex = CameraDepthStencilTargetIndex;
	this->ColorResource = ColorResource;
	this->DepthStencilResource = DepthStencilResource;
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
	//我学会了，每次渲染之前都要手动设置视口
	graphics->pCommandList->RSSetScissorRects(1, &graphics->pRECT);
	graphics->pCommandList->RSSetViewports(1, &graphics->pViewPort);
	//绘制
	graphics->pCommandList->DrawIndexedInstanced(mesh->GetIndicesCount(), 1, 0, 0, 0);
	//渲染目标转换为呈现状态
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	WillClearRenderTarget = false;
}
void Renderer::WaitGpu()
{
	graphics->FlushCommandQueue();
}

void Renderer::ClearCommandList()
{
	THROW_IF_ERROR(graphics->pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(graphics->pCommandList->Reset(graphics->pDirectCmdListAlloc.Get(), nullptr));
}

void Renderer::RefreshSwapChain()
{
	THROW_IF_ERROR(graphics->pIDXGISwapChain->Present(0, 0));
	graphics->CurrentBackBufferIndex = (graphics->CurrentBackBufferIndex + 1) % graphics->mBackBufferCount;
}
void Renderer::ExcuteCommandList()
{
	//提交命令，翻转交换链，清空队列
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

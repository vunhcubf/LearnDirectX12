#include "Renderer.h"

Renderer::Renderer(Graphics* graphics)
{
	this->graphics = graphics;
	Initialize();
	//��ʼ����Ҫ�õ���ȫ������
	//���ü�����
	//�������㻺����ͼ
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

void Renderer::Blit(Texture* src, Texture* dest, D3D12_SHADER_BYTECODE pixel_shader)
{
	BlitMaterial->SetTexture(src);
	BlitMaterial->ResetRTVFormat(src->TextureFormat);
	BlitMaterial->ResetByteCodePS(pixel_shader);
	SetRenderTarget(dest->TextureRTVIndex, graphics->GetSwapchainDSBufferIndex(), dest->TextueResource.Get(), graphics->GetSwapchainDSBuffer());
	DrawMesh(BlitMaterial, FullScreenMesh);
	ExcuteCommandList();
	ClearCommandList();
	BlitMaterial->ResetByteCodePS(Graphics::GetShaderByteCodeFromBlob(BlitPS.Get()));
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
	//����ȾĿ������ΪĿ��״̬
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	if (WillClearRenderTarget) {
		//�����ȾĿ������ģ�建��
		graphics->pCommandList->ClearRenderTargetView(graphics->FetchIndexedViewCpuHandleFromRTVHeap(CameraColorTargetIndex), Colors::LightSteelBlue, 0, nullptr);
		graphics->pCommandList->ClearDepthStencilView(graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}
	//������ȾĿ��
	graphics->pCommandList->OMSetRenderTargets(1, &graphics->FetchIndexedViewCpuHandleFromRTVHeap(CameraColorTargetIndex), true, &graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex));
	//���ó���������������
	ID3D12DescriptorHeap* descriptorHeaps[] = { graphics->CBV_SRV_UAVHeap.Get() };
	graphics->pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//���ø�ǩ��
	graphics->pCommandList->SetGraphicsRootSignature(mat->GetRootSignaturePtr());
	//�󶨶������������
	graphics->pCommandList->IASetVertexBuffers(0, 1, mesh->GetVertexBufferViewPtr());
	graphics->pCommandList->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());
	graphics->pCommandList->IASetIndexBuffer(mesh->GetIndexBufferViewPtr());
	//���ø�ǩ����Ӧ����Դ
	mat->BindRootSignatureResource(graphics);
	graphics->pCommandList->SetPipelineState(mat->GetPipelineStatePtr());
	//ÿ����Ⱦ֮ǰ��Ҫ�ֶ������ӿ�
	graphics->pCommandList->RSSetScissorRects(1, &graphics->pRECT);
	graphics->pCommandList->RSSetViewports(1, &graphics->pViewPort);
	//����
	graphics->pCommandList->DrawIndexedInstanced(mesh->GetIndicesCount(), 1, 0, 0, 0);
	//��ȾĿ��ת��Ϊ����״̬
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	WillClearRenderTarget = false;
}
void Renderer::DrawRenderer(std::unordered_map<std::wstring, Object*> objs)
{
	//����ȾĿ������ΪĿ��״̬
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	//�����ȾĿ������ģ�建��
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
		
		//������ȾĿ��
		graphics->pCommandList->OMSetRenderTargets(1, &graphics->FetchIndexedViewCpuHandleFromRTVHeap(CameraColorTargetIndex), true, &graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex));
		//���ó���������������
		ID3D12DescriptorHeap* descriptorHeaps[] = { graphics->CBV_SRV_UAVHeap.Get() };
		graphics->pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		//���ø�ǩ��
		graphics->pCommandList->SetGraphicsRootSignature(obj.second->material->GetRootSignaturePtr());
		//�󶨶������������
		graphics->pCommandList->IASetVertexBuffers(0, 1, obj.second->mesh->GetVertexBufferViewPtr());
		graphics->pCommandList->IASetPrimitiveTopology(obj.second->mesh->GetPrimitiveTopology());
		graphics->pCommandList->IASetIndexBuffer(obj.second->mesh->GetIndexBufferViewPtr());
		//���ø�ǩ����Ӧ����Դ
		obj.second->material->BindRootSignatureResource(graphics);
		graphics->pCommandList->SetPipelineState(obj.second->material->GetPipelineStatePtr());
		//ÿ����Ⱦ֮ǰ��Ҫ�ֶ������ӿ�
		graphics->pCommandList->RSSetScissorRects(1, &graphics->pRECT);
		graphics->pCommandList->RSSetViewports(1, &graphics->pViewPort);
		//����
		graphics->pCommandList->DrawIndexedInstanced(obj.second->mesh->GetIndicesCount(), 1, 0, 0, 0);
		//��ȾĿ��ת��Ϊ����״̬
	}
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	ExcuteCommandList();
	ClearCommandList();
}
void Renderer::DrawRendererMrt(std::unordered_map<std::wstring, Object*> objs)
{
	if (Rendertargets.size()==0 ||NumRenderTarget<=1)throw std::runtime_error("multi render targets are not set");
	//����ȾĿ������ΪĿ��״̬
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	//�����ȾĿ������ģ�建��
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
		//��ȡ��Ŀ������о��
		for (int i = 0; i < NumRenderTarget; i++) {
			rtv_handles.push_back(graphics->FetchIndexedViewCpuHandleFromRTVHeap(Rendertargets.at(i)->TextureRTVIndex));
		}
		//������ȾĿ��
		graphics->pCommandList->OMSetRenderTargets(NumRenderTarget, rtv_handles.data(), true, &graphics->FetchIndexedViewCpuHandleFromDSVHeap(CameraDepthStencilTargetIndex));
		//���ó���������������
		ID3D12DescriptorHeap* descriptorHeaps[] = { graphics->CBV_SRV_UAVHeap.Get() };
		graphics->pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		//���ø�ǩ��
		graphics->pCommandList->SetGraphicsRootSignature(obj.second->material->GetRootSignaturePtr());
		//�󶨶������������
		graphics->pCommandList->IASetVertexBuffers(0, 1, obj.second->mesh->GetVertexBufferViewPtr());
		graphics->pCommandList->IASetPrimitiveTopology(obj.second->mesh->GetPrimitiveTopology());
		graphics->pCommandList->IASetIndexBuffer(obj.second->mesh->GetIndexBufferViewPtr());
		//���ø�ǩ����Ӧ����Դ
		obj.second->material->BindRootSignatureResource(graphics);
		graphics->pCommandList->SetPipelineState(obj.second->material->GetPipelineStatePtr());
		//ÿ����Ⱦ֮ǰ��Ҫ�ֶ������ӿ�
		graphics->pCommandList->RSSetScissorRects(1, &graphics->pRECT);
		graphics->pCommandList->RSSetViewports(1, &graphics->pViewPort);
		//����
		graphics->pCommandList->DrawIndexedInstanced(obj.second->mesh->GetIndicesCount(), 1, 0, 0, 0);
		//��ȾĿ��ת��Ϊ����״̬
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
	//�ύ�����ն���
	THROW_IF_ERROR(graphics->pCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { graphics->pCommandList.Get() };
	graphics->pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	graphics->FlushCommandQueue();
	THROW_IF_ERROR(graphics->pDirectCmdListAlloc->Reset());
	THROW_IF_ERROR(graphics->pCommandList->Reset(graphics->pDirectCmdListAlloc.Get(), nullptr));
}

void Renderer::ExcuteCommandListWithBlockWithResetSwapChain()
{
	//�ύ�����ն���
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
	//����RTV��������
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = 64;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	THROW_IF_ERROR(graphics->pID3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(graphics->mRTVHeap.GetAddressOf())));
	//����DSV������
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 64;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	THROW_IF_ERROR(graphics->pID3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(graphics->mDSVHeap.GetAddressOf())));
	//���������������������
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 64;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	THROW_IF_ERROR(graphics->pID3DDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(graphics->CBV_SRV_UAVHeap.GetAddressOf())));

	//�������ź󻺳���ͼ
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(graphics->mRTVHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < graphics->mBackBufferCount; i++) {
		THROW_IF_ERROR(graphics->pIDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&(graphics->mSwapChainBuffer[i]))));
		graphics->mSwapChainBufferIndex[i] = graphics->AddViewOnRTVHeap(nullptr, graphics->mSwapChainBuffer[i].Get());
	}
	//�����󻺳��Ӧ�����ģ����ͼ����Դ
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
Renderer::RayTracingAccelerateStructureData Renderer::BuildRayTracingAccelerateStructure(std::unordered_map<std::wstring, Object*> objs) {
	//һ�дӼ�һ�������Ӧһ��blas��tlas�������е�blasһ��
	UINT NumOfObjs = objs.size();
	//���м������������Ϣ
	std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometry_descs(NumOfObjs);
	//ÿ���ײ�ṹ��Ҫ��Ӧһ��buffer
	std::vector<ComPtr<ID3D12Resource>> BottomAccelerateStructureBuffer(NumOfObjs);
	//ÿ���ײ�ṹ�������ṹ��
	std::vector<D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC> BLASDescriptors(NumOfObjs);
	//��ʱ�������
	std::vector<ComPtr<ID3D12Resource>> BLASScratchBuffers(NumOfObjs);
	int i = 0;
	for (auto& obj : objs) {
		//����������
		//����ÿ��obj��Ӧһ���ײ���ٽṹ��ÿ���ײ���ٽṹ��ӳ�䣬������ٽṹ�е�һ��shadertableid
		D3D12_RAYTRACING_GEOMETRY_DESC stModuleGeometryDesc = {};
		stModuleGeometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		stModuleGeometryDesc.Triangles.IndexBuffer = obj.second->mesh->GetIndexBufferGpu()->GetGPUVirtualAddress();
		stModuleGeometryDesc.Triangles.IndexCount = obj.second->mesh->GetIndicesCount();
		stModuleGeometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
		stModuleGeometryDesc.Triangles.Transform3x4 = 0;
		stModuleGeometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		stModuleGeometryDesc.Triangles.VertexCount = obj.second->mesh->GetVerticesCount();
		stModuleGeometryDesc.Triangles.VertexBuffer.StartAddress = obj.second->mesh->GetVertexBufferGpu()->GetGPUVirtualAddress();
		stModuleGeometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Mesh::Vertex_POS_COLOR_UV1_UV2_Normal_Tangent);
		stModuleGeometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
		geometry_descs.at(i)=stModuleGeometryDesc;
		//�ײ���ٽṹ�����ṹ��
		BLASDescriptors.at(i).Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		BLASDescriptors.at(i).Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		BLASDescriptors.at(i).Inputs.NumDescs = 1;
		BLASDescriptors.at(i).Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		BLASDescriptors.at(i).Inputs.pGeometryDescs = &geometry_descs.at(i);
		//��ȡ�ײ�ṹprebuild
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
		graphics->pID3DDevice->GetRaytracingAccelerationStructurePrebuildInfo(&BLASDescriptors.at(i).Inputs, &bottomLevelPrebuildInfo);
		if (bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes <= 0) throw std::runtime_error("bottom accelerate structure prebuild info error:ResultDataMaxSizeInBytes less than 0");
		//Ϊÿ���ײ���ٽṹ����buffer
		graphics->pID3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nullptr,
			IID_PPV_ARGS(BottomAccelerateStructureBuffer.at(i).GetAddressOf())
		);
		BLASDescriptors.at(i).DestAccelerationStructureData = BottomAccelerateStructureBuffer.at(i).Get()->GetGPUVirtualAddress();
		//������ʱ������ݵ�buffer
		graphics->pID3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bottomLevelPrebuildInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(BLASScratchBuffers.at(i).GetAddressOf()));
		BLASDescriptors.at(i).ScratchAccelerationStructureData = BLASScratchBuffers.at(i).Get()->GetGPUVirtualAddress();
		graphics->pCommandList->BuildRaytracingAccelerationStructure(&BLASDescriptors.at(i), 0, nullptr);
		i++;
	}
	////////////////////////////////////////////////////////////////////////////
	//������ٽṹ
	ComPtr<ID3D12Resource> TopAccelerateStructureBuffer;
	ComPtr<ID3D12Resource> TLASScratchBuffers;

 	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC TLASDescriptors = {};
	TLASDescriptors.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	TLASDescriptors.Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	TLASDescriptors.Inputs.NumDescs = NumOfObjs;
	TLASDescriptors.Inputs.pGeometryDescs = nullptr;
	TLASDescriptors.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	//����Ԥ������Ϣ
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
	graphics->pID3DDevice->GetRaytracingAccelerationStructurePrebuildInfo(&TLASDescriptors.Inputs, &topLevelPrebuildInfo);
	if (topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0) throw std::runtime_error("top accelerate structure prebuild info error:ResultDataMaxSizeInBytes less than 0");
	//����TLAS��ʱ���ݵ�buffer
	graphics->pID3DDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(topLevelPrebuildInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(TLASScratchBuffers.GetAddressOf())
	);
	//����TLAS���ݵ�buffer
	graphics->pID3DDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(topLevelPrebuildInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		nullptr,
		IID_PPV_ARGS(TopAccelerateStructureBuffer.GetAddressOf())
	);
	//������buffer�󶨵������ṹ����
	TLASDescriptors.DestAccelerationStructureData = TopAccelerateStructureBuffer.Get()->GetGPUVirtualAddress();
	TLASDescriptors.ScratchAccelerationStructureData = TLASScratchBuffers.Get()->GetGPUVirtualAddress();
	//����ṹ��Ҫ�����ײ�ṹ��ʵ��
	ComPtr<ID3D12Resource> InstanceDescsResource;
	std::vector<D3D12_RAYTRACING_INSTANCE_DESC> InstanceDescriptor(NumOfObjs);
	for (int i = 0; i < NumOfObjs; i++) {
		InstanceDescriptor.at(i).Transform[0][0] = 1;
		InstanceDescriptor.at(i).Transform[0][1] = 0;
		InstanceDescriptor.at(i).Transform[0][2] = 0;
		InstanceDescriptor.at(i).Transform[0][3] = 0;

		InstanceDescriptor.at(i).Transform[1][0] = 0;
		InstanceDescriptor.at(i).Transform[1][1] = 1;
		InstanceDescriptor.at(i).Transform[1][2] = 0;
		InstanceDescriptor.at(i).Transform[1][3] = 0;

		InstanceDescriptor.at(i).Transform[2][0] = 0;
		InstanceDescriptor.at(i).Transform[2][1] = 0;
		InstanceDescriptor.at(i).Transform[2][2] = 1;
		InstanceDescriptor.at(i).Transform[2][3] = 0;

		InstanceDescriptor.at(i).InstanceContributionToHitGroupIndex = i;
		InstanceDescriptor.at(i).Flags = 0;
		InstanceDescriptor.at(i).InstanceID = 0;
		InstanceDescriptor.at(i).InstanceMask = 1;
		InstanceDescriptor.at(i).AccelerationStructure = BottomAccelerateStructureBuffer.at(i).Get()->GetGPUVirtualAddress();
	}
	auto datasize = InstanceDescriptor.size() * sizeof(D3D12_RAYTRACING_INSTANCE_DESC);
	//ʹ���ϴ����ϴ�
	THROW_IF_ERROR(graphics->pID3DDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(datasize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(InstanceDescsResource.GetAddressOf())
	));
	void* MappedData;
	InstanceDescsResource.Get()->Map(0,nullptr,&MappedData);
	memcpy(MappedData, InstanceDescriptor.data(), datasize);
	InstanceDescsResource.Get()->Unmap(0, nullptr);
	TLASDescriptors.Inputs.InstanceDescs = InstanceDescsResource.Get()->GetGPUVirtualAddress();
	//����������ٽṹ
	graphics->pCommandList->BuildRaytracingAccelerationStructure(&TLASDescriptors, 0, nullptr);

	RayTracingAccelerateStructureData data;
	data.BottomAccelerateStructureBuffer = BottomAccelerateStructureBuffer;
	data.BLASDescriptors = BLASDescriptors;
	data.BLASScratchBuffers = BLASScratchBuffers;
	data.geometry_descs = geometry_descs;
	data.TopAccelerateStructureBuffer = TopAccelerateStructureBuffer;
	data.TLASScratchBuffers = TLASScratchBuffers;
	data.TLASDescriptors = TLASDescriptors;
	data.InstanceDescsResource = InstanceDescsResource;
	data.InstanceDescriptor = InstanceDescriptor;
	return data;
}
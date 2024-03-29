#include "Material.h"

void Material::ResetRTVFormats(DXGI_FORMAT* formats)
{
	for (int i = 0; i < this->NumRenderTargets; i++) {
		RtvFormats[i] = formats[i];
	}
}

void Material::RefreshMaterial(Graphics* graphics)
{
	if (PipelineState) { PipelineState->Release(); }
	if (RootSignature) { RootSignature->Release(); }
	CreateRootSignature(graphics);
	//设置管线状态
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { Mesh::InputLayout ,6U};
	psoDesc.pRootSignature = RootSignature;
	psoDesc.VS = ByteCodeVS;
	psoDesc.PS = ByteCodePS;
	psoDesc.RasterizerState = RasterizerState;
	psoDesc.BlendState = BlendState;
	psoDesc.DepthStencilState = DepthStencilState;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = NumRenderTargets;
	for (int i = 0; i < NumRenderTargets; i++) { psoDesc.RTVFormats[i] = RtvFormats[i]; }
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = Graphics::DefaultDSVFormat;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	THROW_IF_ERROR(graphics->pID3DDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PipelineState)));
}

void Material::SetTexture(UINT ViewIndexOfTextures)
{
	slotRootParameterCount++;
	this->ViewIndexOfTextures.push_back(ViewIndexOfTextures);
}

void Material::SetTexture(Texture* texture)
{
	slotRootParameterCount++;
	this->ViewIndexOfTextures.push_back(texture->TextureSRVIndex);
}

void Material::SetConstantBuffer(UINT CBufferViewIndex, ID3D12Resource* CBufferResource)
{
	slotRootParameterCount++;
	ConstantBuffersData.push_back(std::pair(CBufferResource->GetGPUVirtualAddress(), CBufferViewIndex));
}

void Material::CreateRootSignature(Graphics* graphics)
{
	//默认添加顺序 纹理、常量缓冲
	int index = 0;
	for (auto it = ViewIndexOfTextures.begin(); it != ViewIndexOfTextures.end();it++) {
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, index);
		CD3DX12_ROOT_PARAMETER Temp[1];
		Temp[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_ALL);
		slotRootParameters.push_back(Temp[0]);
		index++;
	}
	index = 0;
	for (auto it = ConstantBuffersData.begin(); it != ConstantBuffersData.end(); it++) {
		CD3DX12_ROOT_PARAMETER Temp[1];
		Temp[0].InitAsConstantBufferView(index, 0, D3D12_SHADER_VISIBILITY_ALL);
		slotRootParameters.push_back(Temp[0]);
		index++;
	}
	//创建根签名
	CD3DX12_ROOT_SIGNATURE_DESC rootsigDesc = CD3DX12_ROOT_SIGNATURE_DESC(slotRootParameterCount, slotRootParameters.data(), (UINT)StaticSamplers.size(), StaticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ID3D10Blob* serializedRootSig = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	THROW_IF_ERROR(D3D12SerializeRootSignature(&rootsigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob));
	if (errorBlob != nullptr) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); }
	THROW_IF_ERROR(graphics->pID3DDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));

	if(errorBlob){ errorBlob->Release(); }
	if (serializedRootSig) { serializedRootSig->Release(); }
}

void Material::BindRootSignatureResource(Graphics* graphics)
{
	int index = 0;
	for (auto it = ViewIndexOfTextures.begin(); it != ViewIndexOfTextures.end(); it++) {
		graphics->pCommandList->SetGraphicsRootDescriptorTable(index, graphics->FetchIndexedViewGpuHandleFromCBV_SRV_UAVHeap(*it));
		index++;
	}
	for (auto it = ConstantBuffersData.begin(); it != ConstantBuffersData.end(); it++) {
		graphics->pCommandList->SetGraphicsRootConstantBufferView(index, (*it).first);
		index++;
	}
	slotRootParameters.clear();
	ViewIndexOfTextures.clear();
	ConstantBuffersData.clear();
	slotRootParameterCount = 0;
}

void MaterialResources::AddTexture(Texture* texture)
{
	this->TextureResourcesCollection.push_back(texture);
}

void MaterialResources::AddTextures(std::vector<Texture*> textures)
{
	this->TextureResourcesCollection.insert(TextureResourcesCollection.end(), textures.begin(), textures.end());
}

void MaterialResources::AddFloat(float in)
{
	UINT copysize = 4;
	if (cbufferData!=nullptr) {
		//先把数据放到临时位置
		BYTE* Temp = (BYTE*)malloc(cbufferSize);
		memcpy(Temp, cbufferData, cbufferSize);
		cbufferSize += copysize;
		free(cbufferData);
		cbufferData = (BYTE*)malloc(cbufferSize);
		memcpy(cbufferData,Temp, cbufferSize- copysize);
		memcpy(cbufferData+cbufferSize - copysize, &in, copysize);
		free(Temp);
	}
	else {
		cbufferData = (BYTE*)malloc(copysize);
		memcpy(cbufferData, &in, copysize);
		cbufferSize = copysize;
	}
}

void MaterialResources::AddFloat2(XMFLOAT2 in)
{
	UINT copysize = 4*2;
	if (cbufferData != nullptr) {
		//先把数据放到临时位置
		BYTE* Temp = (BYTE*)malloc(cbufferSize);
		memcpy(Temp, cbufferData, cbufferSize);
		cbufferSize += copysize;
		free(cbufferData);
		cbufferData = (BYTE*)malloc(cbufferSize);
		memcpy(cbufferData, Temp, cbufferSize - copysize);
		memcpy(cbufferData+cbufferSize - copysize, &in, copysize);
		free(Temp);
	}
	else {
		cbufferData = (BYTE*)malloc(copysize);
		memcpy(cbufferData, &in, copysize);
		cbufferSize = copysize;
	}
}

void MaterialResources::AddFloat3(XMFLOAT3 in)
{
	UINT copysize = 4 * 3;
	if (cbufferData != nullptr) {
		//先把数据放到临时位置
		BYTE* Temp = (BYTE*)malloc(cbufferSize);
		memcpy(Temp, cbufferData, cbufferSize);
		cbufferSize += copysize;
		free(cbufferData);
		cbufferData = (BYTE*)malloc(cbufferSize);
		memcpy(cbufferData, Temp, cbufferSize - copysize);
		memcpy(cbufferData+cbufferSize - copysize, &in, copysize);
		free(Temp);
	}
	else {
		cbufferData = (BYTE*)malloc(copysize);
		memcpy(cbufferData, &in, copysize);
		cbufferSize = copysize;
	}
}

void MaterialResources::AddFloat4(XMFLOAT4 in)
{
	UINT copysize = 4 * 4;
	if (cbufferData != nullptr) {
		//先把数据放到临时位置
		BYTE* Temp = (BYTE*)malloc(cbufferSize);
		memcpy(Temp, cbufferData, cbufferSize);
		cbufferSize += copysize;
		free(cbufferData);
		cbufferData = (BYTE*)malloc(cbufferSize);
		memcpy(cbufferData, Temp, cbufferSize - copysize);
		memcpy(cbufferData+cbufferSize - copysize, &in, copysize);
		free(Temp);
	}
	else {
		cbufferData = (BYTE*)malloc(copysize);
		memcpy(cbufferData, &in, copysize);
		cbufferSize = copysize;
	}
}

void MaterialResources::AddFloat4x4(XMFLOAT4 in)
{
	UINT copysize = 4 * 16;
	if (cbufferData != nullptr) {
		//先把数据放到临时位置
		BYTE* Temp = (BYTE*)malloc(cbufferSize);
		memcpy(Temp, cbufferData, cbufferSize);
		cbufferSize += copysize;
		free(cbufferData);
		cbufferData = (BYTE*)malloc(cbufferSize);
		memcpy(cbufferData, Temp, cbufferSize - copysize);
		memcpy(cbufferData+cbufferSize - copysize, &in, copysize);
		free(Temp);
	}
	else {
		cbufferData = (BYTE*)malloc(copysize);
		memcpy(cbufferData, &in, copysize);
		cbufferSize = copysize;
	}
}

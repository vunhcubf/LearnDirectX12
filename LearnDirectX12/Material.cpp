#include "Material.h"

void Material::RefreshMaterial(Graphics* graphics)
{
	CreateRootSignature(graphics);
	//设置管线状态
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { Mesh::InputLayout ,6 };
	psoDesc.pRootSignature = RootSignature.Get();
	psoDesc.VS = *ByteCodeVS;
	psoDesc.PS = *ByteCodePS;
	psoDesc.RasterizerState = RasterizerState;
	psoDesc.BlendState = BlendState;
	psoDesc.DepthStencilState = DepthStencilState;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = NumRenderTargets;
	for (int i = 0; i < NumRenderTargets; i++) { psoDesc.RTVFormats[i] = Graphics::DefaultRTVFormat; }
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = Graphics::DefaultDSVFormat;
	psoDesc.SampleMask = UINT_MAX;
	THROW_IF_ERROR(graphics->pID3DDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PipelineState)));
}

void Material::SetTexture(UINT ViewIndexOfTextures)
{
	slotRootParameterCount++;
	this->ViewIndexOfTextures.push_back(ViewIndexOfTextures);
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
	CD3DX12_ROOT_SIGNATURE_DESC rootsigDesc = CD3DX12_ROOT_SIGNATURE_DESC(slotRootParameterCount, slotRootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3D10Blob> serializedRootSig = nullptr;
	ComPtr<ID3D10Blob> errorBlob = nullptr;
	THROW_IF_ERROR(D3D12SerializeRootSignature(&rootsigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf()));
	if (errorBlob != nullptr) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); }
	THROW_IF_ERROR(graphics->pID3DDevice->CreateRootSignature(0, serializedRootSig.Get()->GetBufferPointer(), serializedRootSig.Get()->GetBufferSize(), IID_PPV_ARGS(RootSignature.GetAddressOf())));
}

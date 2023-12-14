#pragma once
#include "Graphics.h"
#include "Mesh.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include <bitset>
#include <vector>

struct MaterialResources {
public:
	void AddTexture(Texture* texture);
	void AddTextures(std::vector<Texture*> textures);
	void AddFloat(float in);
	void AddFloat2(XMFLOAT2 in);
	void AddFloat3(XMFLOAT3 in);
	void AddFloat4(XMFLOAT4 in);
	void AddFloat4x4(XMFLOAT4 in);
	std::vector<Texture*> TextureResourcesCollection;
	UINT cbufferSize=0;
	BYTE* cbufferData=nullptr;
};

class Material {
private:
	ID3D12RootSignature* RootSignature = nullptr;
	ID3D12PipelineState* PipelineState = nullptr;
	D3D12_SHADER_BYTECODE ByteCodeVS;
	D3D12_SHADER_BYTECODE ByteCodePS;

	D3D12_RASTERIZER_DESC RasterizerState;
	D3D12_BLEND_DESC BlendState;
	D3D12_DEPTH_STENCIL_DESC DepthStencilState;
	DXGI_FORMAT DSVFormat;
	DXGI_FORMAT RTVFormat;
	UINT NumRenderTargets;
	UINT slotRootParameterCount = 0;
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameters;
	std::vector<UINT> ViewIndexOfTextures;
	std::vector<std::pair<D3D12_GPU_VIRTUAL_ADDRESS, UINT>> ConstantBuffersData;
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> StaticSamplers = { 
		CD3DX12_STATIC_SAMPLER_DESC(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP),

		CD3DX12_STATIC_SAMPLER_DESC(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP),

		CD3DX12_STATIC_SAMPLER_DESC(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP),

		CD3DX12_STATIC_SAMPLER_DESC(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP),

		CD3DX12_STATIC_SAMPLER_DESC(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP),

		CD3DX12_STATIC_SAMPLER_DESC(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP),
	};
public:
	//材质中对应的资源
	ConstantBuffer32bits* cbuffer32bits;
	BYTE* CbufferDataPtr = nullptr;
	std::vector<Texture*> TextureResources;
	Material(D3D12_SHADER_BYTECODE ByteCodeVS,
		D3D12_SHADER_BYTECODE ByteCodePS,
		Graphics* graphics,
		MaterialResources resources,
		UINT NumRenderTargets,
		D3D12_RASTERIZER_DESC RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		D3D12_BLEND_DESC BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
		D3D12_DEPTH_STENCIL_DESC DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
		DXGI_FORMAT DSVFormat= Graphics::DefaultDSVFormat,
		DXGI_FORMAT RTVFormat= Graphics::DefaultRTVFormat) {

		this->ByteCodePS = ByteCodePS;
		this->ByteCodeVS = ByteCodeVS;
		this->RasterizerState = RasterizerState;
		this->BlendState = BlendState;
		this->DepthStencilState = DepthStencilState;
		this->DSVFormat = DSVFormat;
		this->RTVFormat = RTVFormat;
		this->NumRenderTargets = NumRenderTargets;

		this->TextureResources = resources.TextureResourcesCollection;
		this->cbuffer32bits = new ConstantBuffer32bits(graphics, resources.cbufferSize);
		cbuffer32bits->CopyData(resources.cbufferData);
		CbufferDataPtr = resources.cbufferData;
	}
	void ResetByteCodeVS(D3D12_SHADER_BYTECODE ByteCodeVS) { this->ByteCodeVS = ByteCodeVS; }
	void ResetByteCodePS(D3D12_SHADER_BYTECODE ByteCodePS) { this->ByteCodeVS = ByteCodePS; }
	void ResetNumRenderTargets(UINT NumRenderTargets) { this->NumRenderTargets = NumRenderTargets; }
	void ResetRasterizerState(D3D12_RASTERIZER_DESC RasterizerState){ this->RasterizerState = RasterizerState; }
	void ResetBlendState(D3D12_BLEND_DESC BlendState) { this->BlendState = BlendState; }
	void ResetDepthStencilState(D3D12_DEPTH_STENCIL_DESC DepthStencilState) { this->DepthStencilState = DepthStencilState; }
	void ResetDSVFormat(DXGI_FORMAT DSVFormat) { this->DSVFormat = DSVFormat; }
	void ResetRTVFormat(DXGI_FORMAT RTVFormat) { this->RTVFormat = RTVFormat; }
	void RefreshMaterial(Graphics* graphics);

	void SetTexture(UINT ViewIndexOfTextures);
	void SetTexture(Texture* texture);
	void SetConstantBuffer(UINT CBufferViewIndex, ID3D12Resource* CBufferResource);

	template<typename T>
	void SetConstantBuffer(ConstantBuffer<T>* cbuffer)
	{
		slotRootParameterCount++;
		ConstantBuffersData.push_back(std::pair(cbuffer->GetCBufferUploadBufferPtr()->GetGPUVirtualAddress(), cbuffer->GetCBufferViewIndex()));
	}
	void SetConstantBuffer(ConstantBuffer32bits* cbuffer) {
		slotRootParameterCount++;
		ConstantBuffersData.push_back(std::pair(cbuffer->GetCBufferUploadBufferPtr()->GetGPUVirtualAddress(), cbuffer->GetCBufferViewIndex()));
	}

	void CreateRootSignature(Graphics* graphics);
	void BindRootSignatureResource(Graphics* graphics);
	ID3D12RootSignature* GetRootSignaturePtr()const {return RootSignature;}
	ID3D12PipelineState* GetPipelineStatePtr()const { return PipelineState; }
	~Material() {
		if(RootSignature!=nullptr)RootSignature->Release();
		if (PipelineState != nullptr)PipelineState->Release();
		slotRootParameters.clear();
		ViewIndexOfTextures.clear();
		ConstantBuffersData.clear();
		slotRootParameters.shrink_to_fit();
		ViewIndexOfTextures.shrink_to_fit();
		ConstantBuffersData.shrink_to_fit();
		if(cbuffer32bits)delete cbuffer32bits;
		if(CbufferDataPtr)free(CbufferDataPtr);
	}
};



#pragma once
#include "Graphics.h"
#include "Mesh.h"
#include "ConstantBuffer.h"
#include <bitset>

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

public:
	Material(D3D12_SHADER_BYTECODE ByteCodeVS,
		D3D12_SHADER_BYTECODE ByteCodePS,
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
	void SetConstantBuffer(UINT CBufferViewIndex, ID3D12Resource* CBufferResource);

	template<typename T>
	void SetConstantBuffer(ConstantBuffer<T>* cbuffer)
	{
		slotRootParameterCount++;
		ConstantBuffersData.push_back(std::pair(cbuffer->GetCBufferUploadBufferPtr()->GetGPUVirtualAddress(), cbuffer->GetCBufferViewIndex()));
	}

	//void Set32BitThings();
	void CreateRootSignature(Graphics* graphics);
	void BindRootSignatureResource(Graphics* graphics);
	ID3D12RootSignature* GetRootSignaturePtr()const {return RootSignature;}
	ID3D12PipelineState* GetPipelineStatePtr()const { return PipelineState; }
	~Material() {
		RootSignature->Release();
		PipelineState->Release();
		slotRootParameters.clear();
		ViewIndexOfTextures.clear();
		ConstantBuffersData.clear();
		slotRootParameters.shrink_to_fit();
		ViewIndexOfTextures.shrink_to_fit();
		ConstantBuffersData.shrink_to_fit();
	}
};



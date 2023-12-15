#include "Texture.h"

Texture::Texture(D3D12_SHADER_RESOURCE_VIEW_DESC* descriptor, D3D12_RESOURCE_DESC textureDesc,Graphics* graphics) {
	D3D12_CLEAR_VALUE clear_value;
	float clear_color[4] = { 0.5,0.5,0.5,0.5 };
	clear_value.Format = textureDesc.Format;
	clear_value.Color[0] = 0.5;
	clear_value.Color[1] = 0.5;
	clear_value.Color[2] = 0.5;
	clear_value.Color[3] = 0.5;

	THROW_IF_ERROR(graphics->pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,&textureDesc,D3D12_RESOURCE_STATE_COMMON,&clear_value,IID_PPV_ARGS(&this->TextueResource)));
	TextureFormat = textureDesc.Format;
	this->TextureSRVIndex = graphics->AddViewOnSRVHeap(descriptor, this->TextueResource.Get());
}

Texture::Texture(D3D12_RENDER_TARGET_VIEW_DESC* rtv_desc, D3D12_SHADER_RESOURCE_VIEW_DESC* srv_desc, D3D12_RESOURCE_DESC textureDesc, Graphics* graphics)
{
	D3D12_CLEAR_VALUE clear_value;
	float clear_color[4] = { 0.5,0.5,0.5,0.5 };
	clear_value.Format = textureDesc.Format;
	clear_value.Color[0] = 0.5;
	clear_value.Color[1] = 0.5;
	clear_value.Color[2] = 0.5;
	clear_value.Color[3] = 0.5;

	THROW_IF_ERROR(graphics->pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COMMON, &clear_value, IID_PPV_ARGS(&this->TextueResource)));
	TextureFormat = textureDesc.Format;
	this->TextureSRVIndex = graphics->AddViewOnSRVHeap(srv_desc, this->TextueResource.Get());
	this->TextureRTVIndex = graphics->AddViewOnRTVHeap(rtv_desc, this->TextueResource.Get());
}

Texture::Texture(int width,int height,int arraysize,int miplevel,DXGI_FORMAT Format, D3D12_SRV_DIMENSION dimension, D3D12_TEX2D_SRV type, Graphics* graphics)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC descriptor;
	descriptor.Format = Format;
	TextureFormat = Format;
	descriptor.ViewDimension = dimension;
	descriptor.Texture2D = type;
	descriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	
	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(Format, width, height, arraysize, miplevel);

	D3D12_CLEAR_VALUE clear_value;
	float clear_color[4] = { 0.5,0.5,0.5,0.5 };
	clear_value.Format = Format;
	clear_value.Color[0] = 0.5;
	clear_value.Color[1] = 0.5;
	clear_value.Color[2] = 0.5;
	clear_value.Color[3] = 0.5;

	THROW_IF_ERROR(graphics->pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COMMON, &clear_value, IID_PPV_ARGS(&this->TextueResource)));

	TextureSRVIndex = graphics->AddViewOnSRVHeap(&descriptor, this->TextueResource.Get());
}

Texture::Texture(std::wstring filepath, Graphics* graphics)
{
	THROW_IF_ERROR(CreateDDSTextureFromFile12(graphics->pID3DDevice.Get(), graphics->pCommandList.Get(), filepath.c_str(), this->TextueResource, UploadBuffer));
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TextueResource->GetDesc().Format;
	TextureFormat = srvDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = TextueResource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	TextureSRVIndex = graphics->AddViewOnSRVHeap(&srvDesc, TextueResource.Get());
}

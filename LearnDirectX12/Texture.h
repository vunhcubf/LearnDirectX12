#pragma once
#include "Graphics.h"
#include "DDSTextureLoader.h"
#include <string>
class Texture {
public:
    union SRV_RESOURCE_TYPE
    {
        D3D12_BUFFER_SRV Buffer;
        D3D12_TEX1D_SRV Texture1D;
        D3D12_TEX1D_ARRAY_SRV Texture1DArray;
        D3D12_TEX2D_SRV Texture2D;
        D3D12_TEX2D_ARRAY_SRV Texture2DArray;
        D3D12_TEX2DMS_SRV Texture2DMS;
        D3D12_TEX2DMS_ARRAY_SRV Texture2DMSArray;
        D3D12_TEX3D_SRV Texture3D;
        D3D12_TEXCUBE_SRV TextureCube;
        D3D12_TEXCUBE_ARRAY_SRV TextureCubeArray;
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_SRV RaytracingAccelerationStructure;
    };
	UINT TextureViewIndex;
    std::wstring Name;
	ComPtr<ID3D12Resource> TextueResource=nullptr;
    ComPtr<ID3D12Resource> UploadBuffer=nullptr;
	Texture(D3D12_SHADER_RESOURCE_VIEW_DESC* descriptor, CD3DX12_RESOURCE_DESC textureDesc, Graphics* graphics);
	Texture(int width, int height, int arraysize, int miplevel, DXGI_FORMAT Format, D3D12_SRV_DIMENSION dimension, D3D12_TEX2D_SRV type, Graphics* graphics);
    Texture(std::wstring filepath, Graphics* graphics);
};
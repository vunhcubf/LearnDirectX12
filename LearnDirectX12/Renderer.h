#pragma once
#include "Graphics.h"
#include "Material.h"
#include "Mesh.h"

class Renderer {
	Graphics* graphics;
	UINT CameraColorTargetIndex;
	UINT CameraDepthStencilTargetIndex;
	ID3D12Resource* ColorResource;
	ID3D12Resource* DepthStencilResource;
	void SetRenderTarget(UINT CameraColorTargetIndex, UINT CameraDepthStencilTargetIndex, ID3D12Resource* ColorResource, ID3D12Resource* DepthStencilResource);
	void ClearRenderTarget();
	void DrawMesh(Material* mat,Mesh* mesh);

	bool WillClearRenderTarget = false;
};
#pragma once
#include "Graphics.h"
#include "Material.h"
#include "Mesh.h"

class Renderer {
//public:
//	struct CBufferPerFrame {
//		CBufferPerFrame(XMFLOAT4X4& CameraVPMatrix) :CameraVPMatrix(CameraVPMatrix) {};
//		CBufferPerFrame(XMMATRIX& CameraVPMatrix) :CameraVPMatrix(Graphics::MatrixToFloat4x4(CameraVPMatrix)) {};
//		CBufferPerFrame(XMFLOAT4X4&& CameraVPMatrix) :CameraVPMatrix(CameraVPMatrix) {};
//		CBufferPerFrame(XMMATRIX&& CameraVPMatrix) :CameraVPMatrix(Graphics::MatrixToFloat4x4(CameraVPMatrix)) {};
//		XMFLOAT4X4 CameraVPMatrix;
//	};
public:
	Graphics* graphics;
	UINT CameraColorTargetIndex=0;
	UINT CameraDepthStencilTargetIndex=0;
	ID3D12Resource* ColorResource = nullptr;
	ID3D12Resource* DepthStencilResource = nullptr;
	Renderer(Graphics* graphics) :graphics(graphics) { Initialize(); };
	void SetRenderTarget(UINT CameraColorTargetIndex, UINT CameraDepthStencilTargetIndex, ID3D12Resource* ColorResource, ID3D12Resource* DepthStencilResource);
	void ClearRenderTarget();
	void DrawMesh(Material* mat, Mesh* mesh);
	void ExcuteCommandList();
	void ExcuteCommandListWithBlock();
	void ExcuteCommandListWithBlockWithResetSwapChain();
	void WaitGpu();
	void ClearCommandList();
	void RefreshSwapChain();
	void Initialize();
	bool WillClearRenderTarget = false;
};
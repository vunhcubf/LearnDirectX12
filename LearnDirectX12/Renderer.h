#pragma once
#include "Graphics.h"
#include "Material.h"
#include "Mesh.h"
#include "Object.h"
#include <unordered_map>
#include <string>

class Renderer {
public:
	Graphics* graphics;
	UINT CameraColorTargetIndex=0;
	UINT CameraDepthStencilTargetIndex=0;
	ID3D12Resource* ColorResource = nullptr;
	ID3D12Resource* DepthStencilResource = nullptr;
	Renderer(Graphics* graphics);

	//后处理用的
	Mesh* FullScreenMesh;
	Material* BlitMaterial;
	void Blit(Texture* src,Texture* dest);
	void Blit(Texture* src, Texture* dest, D3D12_SHADER_BYTECODE pixel_shader);
	void Blit(Texture* src);
	ComPtr<ID3DBlob> BlitVS;
	ComPtr<ID3DBlob> BlitPS;
	//Mrt用到的渲染目标
	std::vector<Texture*> Rendertargets;
	UINT NumRenderTarget;

	void SetRenderTarget(UINT CameraColorTargetIndex, UINT CameraDepthStencilTargetIndex, ID3D12Resource* ColorResource, ID3D12Resource* DepthStencilResource);
	void SetRenderTarget(Texture* dest, UINT CameraDepthStencilTargetIndex, ID3D12Resource* DepthStencilResource);
	void SetRenderTarget(std::vector<Texture*> dest, UINT NumRenderTarget, UINT CameraDepthStencilTargetIndex, ID3D12Resource* DepthStencilResource);
	void ClearRenderTarget();
	void DrawMesh(Material* mat, Mesh* mesh);
	void DrawRenderer(std::unordered_map<std::wstring, Object*> objs);
	void DrawRendererMrt(std::unordered_map<std::wstring, Object*> objs);
	void ExcuteCommandList();
	void ExcuteCommandListWithBlock();
	void ExcuteCommandListWithBlockWithResetSwapChain();
	void WaitGpu();
	void ClearCommandList();
	void ClearCommandAllocater();
	void RefreshSwapChain();
	void Initialize();
	bool WillClearRenderTarget = false;
	~Renderer() {
		delete FullScreenMesh;
	}
	//以下是光线追踪的部分
	struct RayTracingAccelerateStructureData {
		std::vector<ComPtr<ID3D12Resource>> BottomAccelerateStructureBuffer;
		std::vector<D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC> BLASDescriptors;
		std::vector<ComPtr<ID3D12Resource>> BLASScratchBuffers;
		std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometry_descs;
		ComPtr<ID3D12Resource> TopAccelerateStructureBuffer;
		ComPtr<ID3D12Resource> TLASScratchBuffers;
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC TLASDescriptors;
		ComPtr<ID3D12Resource> InstanceDescsResource;
		std::vector<D3D12_RAYTRACING_INSTANCE_DESC> InstanceDescriptor;
	};
	RayTracingAccelerateStructureData BuildRayTracingAccelerateStructure(std::unordered_map<std::wstring,Object*> obj);
};
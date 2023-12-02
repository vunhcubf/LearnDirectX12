#include "Mesh.h"

void Mesh::InitializeAndUploadMesh(Graphics* graphics,bool UseDefaultCube)
{
	//设置几何体
	//创建顶点缓冲视图
	ID3D12Resource* VertexBufferUploaderTemp = nullptr;
	const UINT64 vbByteSize = CountOfVertices * sizeof(Vertex_POS_COLOR_UV1_UV2_Normal_Tangent);
	VertexBufferGpu = graphics->CreateDefaultBuffer(UseDefaultCube? DefaultCube_Vertices: Vertices.data(), vbByteSize, VertexBufferUploaderTemp);
	VertexBufferUploaderTemp->Release();

	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = VertexBufferGpu->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(Vertex_POS_COLOR_UV1_UV2_Normal_Tangent);
	vbv.SizeInBytes = vbByteSize;
	VertexBufferView = vbv;
	//创建索引缓冲和视图
	ID3D12Resource* IndexBufferUploaderTemp = nullptr;
	const UINT ibByteSize = CountOfIndices * sizeof(UINT);
	IndexBufferGpu = graphics->CreateDefaultBuffer(UseDefaultCube?DefaultCube_Indices:Indices.data(), ibByteSize, IndexBufferUploaderTemp);
	IndexBufferUploaderTemp->Release();

	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = IndexBufferGpu->GetGPUVirtualAddress();
	ibv.Format = DXGI_FORMAT_R32_UINT;
	ibv.SizeInBytes = ibByteSize;
	IndexBufferView = ibv;
	//创建常量缓冲视图
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = CBufferPerObjectForUpload->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = CALC_CBUFFER_BYTE_SIZE(sizeof(CBufferPerObject));
	CBufferPerObjectViewIndex=graphics->AddViewOnCBVHeap(&cbvDesc);
}

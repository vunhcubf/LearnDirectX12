#pragma once
#include "Graphics.h"

class Mesh {
public:
	struct Vertex_POS_COLOR_UV1_UV2_Normal_Tangent {
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
		XMFLOAT2 Uv1;
		XMFLOAT2 Uv2;
		XMFLOAT3 Normal;
		XMFLOAT3 Tangent;
	};
	struct CBufferPerObject {
		XMFLOAT4X4 ObjToWorldMatrix;
		XMFLOAT4 OtherPerObjectData;
	};

	ComPtr<ID3D12Resource> VertexBufferGpu = nullptr;
	ComPtr<ID3D12Resource> IndexBufferGpu = nullptr;
	ComPtr<ID3D12Resource> CBufferPerObjectForUpload = nullptr;//这个Cbuffer时用于上传的上传堆
	BYTE* CBufferDataPtr = nullptr;//实际存放内容的地方，中转站
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	UINT CBufferPerObjectViewIndex;
	UINT CountOfVertices = 8;
	UINT CountOfIndices = 36;
	std::vector<Vertex_POS_COLOR_UV1_UV2_Normal_Tangent> Vertices;
	std::vector<UINT> Indices;
	static constexpr D3D12_INPUT_ELEMENT_DESC InputLayout[6] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD0",0,DXGI_FORMAT_R32G32_FLOAT,0,28,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD1",0,DXGI_FORMAT_R32G32_FLOAT,0,36,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,44,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,56,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};
	static constexpr Vertex_POS_COLOR_UV1_UV2_Normal_Tangent DefaultCube_Vertices[8] = {
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(0.f, 0.f, 0.f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(0.196078449f, 0.803921640f, 0.196078449f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(0.501960814f, 0.f, 0.f, 1.f)	,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(1.f, 0.894117713f, 0.709803939f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(0.992156923f, 0.960784376f, 0.901960850f, 1.f) ,XMFLOAT2(0,0),	XMFLOAT2(0,0),	XMFLOAT3(0,0,1),	XMFLOAT3(0,0,1)},
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(0.690196097f, 0.878431439f, 0.901960850f, 1.f) ,XMFLOAT2(0,0),	XMFLOAT2(0,0),	XMFLOAT3(0,0,1),	XMFLOAT3(0,0,1)},
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(0.180392161f, 0.545098066f, 0.341176480f, 1.f) ,XMFLOAT2(0,0),	XMFLOAT2(0,0),	XMFLOAT3(0,0,1),	XMFLOAT3(0,0,1)}
	};
	static constexpr UINT DefaultCube_Indices[36] = { 0, 1, 2,0, 2, 3,4, 6, 5,4, 7, 6,4, 5, 1,4, 1, 0,3, 2, 6,3, 6, 7,1, 5, 6,1, 6, 2,4, 0, 3,4, 3, 7 };
	void InitializeAndUploadMesh(Graphics* graphics, bool UseDefaultCube = true);
	Mesh(Graphics* graphics,const std::vector<Vertex_POS_COLOR_UV1_UV2_Normal_Tangent>& Vertices, const std::vector<UINT>& Indices, UINT CountOfVertices, UINT CountOfIndices, const CBufferPerObject& CBufferData) {
		this->Vertices = Vertices;
		this->Indices = Indices;
		this->CountOfVertices = CountOfVertices;
		this->CountOfIndices = CountOfIndices;
		//给CBufferDataPtr分配内存
		CBufferDataPtr = (BYTE*)new CBufferPerObject;
		//上传堆应在构造时初始化
		UINT CBufferByteSize = CALC_CBUFFER_BYTE_SIZE(sizeof(CBufferPerObject));
		THROW_IF_ERROR(graphics->pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(CBufferByteSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(CBufferPerObjectForUpload.GetAddressOf())));
		
		//将常量缓冲的内容和CBufferDataPtr绑定
		THROW_IF_ERROR(CBufferPerObjectForUpload->Map(0, nullptr, reinterpret_cast<void**>(&CBufferDataPtr)));
		//把内容复制过来
		memcpy(CBufferDataPtr,&CBufferData,sizeof(CBufferPerObject));
		CBufferPerObjectForUpload->Unmap(0,nullptr);
	}
	void Update(std::vector<Vertex_POS_COLOR_UV1_UV2_Normal_Tangent>& Vertices, std::vector<UINT>& Indices, UINT CountOfVertices, UINT CountOfIndices, CBufferPerObject& CBufferData) {
		this->Vertices = Vertices;
		this->Indices = Indices;
		this->CountOfVertices = CountOfVertices;
		this->CountOfIndices = CountOfIndices;
		VertexBufferGpu.Reset();
		CBufferPerObjectForUpload.Reset();
		IndexBufferGpu.Reset();
		
		//将常量缓冲的内容和CBufferDataPtr绑定
		THROW_IF_ERROR(CBufferPerObjectForUpload->Map(0, nullptr, reinterpret_cast<void**>(&CBufferDataPtr)));
		//把内容复制过来
		if (CBufferDataPtr != nullptr) {
			memcpy(CBufferDataPtr, &CBufferData, sizeof(CBufferPerObject));
		}
		CBufferPerObjectForUpload->Unmap(0, nullptr);
	}
	Mesh& operator=(const Mesh&) = delete;
	Mesh(const Mesh& Mesh) = delete;
	Mesh(const Mesh&& Mesh) = delete;
	~Mesh() {
		Vertices.clear();
		Indices.clear();
		Vertices.shrink_to_fit();
		Indices.shrink_to_fit();
		VertexBufferGpu.Reset();
		CBufferPerObjectForUpload.Reset();
		IndexBufferGpu.Reset();
		delete CBufferDataPtr;
	}
};
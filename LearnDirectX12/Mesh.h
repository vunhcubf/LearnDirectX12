#pragma once
#include "Graphics.h"
#include "ConstantBuffer.h"
#include <array>

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
		CBufferPerObject() {};
		CBufferPerObject(XMFLOAT4X4 ObjToWorldMatrix, XMFLOAT4 OtherPerObjectData) :ObjToWorldMatrix(ObjToWorldMatrix), OtherPerObjectData(OtherPerObjectData) {};
		XMFLOAT4X4 ObjToWorldMatrix;
		XMFLOAT4 OtherPerObjectData;
	};
private:
	ConstantBuffer<CBufferPerObject>* cbufferPerobj;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ComPtr<ID3D12Resource> VertexBufferGpu = nullptr;
	ComPtr<ID3D12Resource> IndexBufferGpu = nullptr;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	UINT CBufferPerObjectViewIndex;
	UINT CountOfVertices = 24;
	UINT CountOfIndices = 36;
	std::vector<Vertex_POS_COLOR_UV1_UV2_Normal_Tangent> Vertices;
	std::vector<UINT> Indices;
public:
	static constexpr D3D12_INPUT_ELEMENT_DESC InputLayout[6]={
		{"POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0},
		{"COLOR",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	12,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0},
		{"TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,	0,	28,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0},
		{"TEXCOORD",	1,	DXGI_FORMAT_R32G32_FLOAT,	0,	36,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0},
		{"NORMAL",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	44,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0},
		{"TANGENT",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	56,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0}
	};
	static constexpr Vertex_POS_COLOR_UV1_UV2_Normal_Tangent DefaultCube_Vertices[24] = {
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},

		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},

		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(0,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) ,XMFLOAT2(1,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1)},
	};
	//{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT2(0, 0), XMFLOAT2(0, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(0, 0, 1)},
	//{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(0.f, 0.f, 0.f, 1.f) ,XMFLOAT2(1,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1) },
	//{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(0.196078449f, 0.803921640f, 0.196078449f, 1.f) ,XMFLOAT2(1,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1) },
	//{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(0.501960814f, 0.f, 0.f, 1.f)	,XMFLOAT2(0,0)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1) },
	//{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(1.f, 0.894117713f, 0.709803939f, 1.f) ,XMFLOAT2(0,1)	,XMFLOAT2(0,0)	,XMFLOAT3(0,0,1)	,XMFLOAT3(0,0,1) },
	//{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(0.992156923f, 0.960784376f, 0.901960850f, 1.f) ,XMFLOAT2(1,1),	XMFLOAT2(0,0),	XMFLOAT3(0,0,1),	XMFLOAT3(0,0,1) },
	//{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(0.690196097f, 0.878431439f, 0.901960850f, 1.f) ,XMFLOAT2(1,1),	XMFLOAT2(0,0),	XMFLOAT3(0,0,1),	XMFLOAT3(0,0,1) },
	//{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(0.180392161f, 0.545098066f, 0.341176480f, 1.f) ,XMFLOAT2(0,1),	XMFLOAT2(0,0),	XMFLOAT3(0,0,1),	XMFLOAT3(0,0,1) }
	/*static constexpr UINT DefaultCube_Indices[36] = { 0, 1, 2,0, 2, 3,4, 6, 5,4, 7, 6,4, 5, 1,4, 1, 0,3, 2, 6,3, 6, 7,1, 5, 6,1, 6, 2,4, 0, 3,4, 3, 7 };*/
	static constexpr UINT DefaultCube_Indices[36] = {0,1,2,1,2,3  ,4,5,6,5,6,7  ,8,9,10,9,10,11  ,12,13,14,13,14,15  ,16,17,18,17,18,19  ,20,21,22,21,22,23};

	void InitializeAndUploadMesh(Graphics* graphics, bool UseDefaultCube = true);
	Mesh(Graphics* graphics, const std::vector<Vertex_POS_COLOR_UV1_UV2_Normal_Tangent>& Vertices, const std::vector<UINT>& Indices, UINT CountOfVertices, UINT CountOfIndices, CBufferPerObject* CBufferData, D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
		this->Vertices = Vertices;
		this->Indices = Indices;
		this->CountOfVertices = CountOfVertices;
		this->CountOfIndices = CountOfIndices;
		this->PrimitiveTopology = PrimitiveTopology;
		cbufferPerobj = new ConstantBuffer<CBufferPerObject>(graphics);
		cbufferPerobj->CopyData(CBufferData);
	}
	Mesh(Graphics* graphics, CBufferPerObject* CBufferData, D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
		this->PrimitiveTopology = PrimitiveTopology;
		cbufferPerobj = new ConstantBuffer<CBufferPerObject>(graphics);
		cbufferPerobj->CopyData(CBufferData);
	}
	void ResetCBufferData(CBufferPerObject* CBufferData) {
		cbufferPerobj->CopyData(CBufferData);
	}
	void ResetVertices(std::vector<Vertex_POS_COLOR_UV1_UV2_Normal_Tangent>& Vertices, UINT CountOfVertices) {
		this->Vertices = Vertices;
		this->CountOfVertices = CountOfVertices;
	}
	void ResetIndices(std::vector<UINT>& Indices, UINT CountOfIndices) {
		this->Indices = Indices;
		this->CountOfIndices = CountOfIndices;
	}
	Mesh(const Mesh& Mesh) = delete;
	Mesh(const Mesh&& Mesh) = delete;
	~Mesh() {
		Vertices.clear();
		Indices.clear();
		Vertices.shrink_to_fit();
		Indices.shrink_to_fit();
		VertexBufferGpu.Reset();
		IndexBufferGpu.Reset();
		delete cbufferPerobj;
	}
	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferViewPtr() { return &VertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferViewPtr() { return &IndexBufferView; }
	D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() { return PrimitiveTopology; }
	UINT GetCBufferPerObjViewIndex() {return CBufferPerObjectViewIndex;}
	ID3D12Resource* GetCBufferPerObjectForUploadPtr() {return cbufferPerobj->GetCBufferUploadBufferPtr();}
	UINT GetIndicesCount() { return CountOfIndices; }
};
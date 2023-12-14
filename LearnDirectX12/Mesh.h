#pragma once
#include "Graphics.h"
#include "ConstantBuffer.h"
#include <array>
#include <vector>
#include <string>
#include <fstream>

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
	ComPtr<ID3D12Resource> VertexBufferGpu ;
	ComPtr<ID3D12Resource> IndexBufferGpu ;
	ComPtr<ID3D12Resource> VertexBufferUploaderTemp;
	ComPtr<ID3D12Resource> IndexBufferUploaderTemp;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	UINT CBufferPerObjectViewIndex;
	UINT CountOfVertices = 24;
	UINT CountOfIndices = 36;
	std::vector<Vertex_POS_COLOR_UV1_UV2_Normal_Tangent> Vertices;
	std::vector<UINT> Indices;
public:
	static constexpr D3D12_INPUT_ELEMENT_DESC InputLayout[6] = {
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
	static constexpr UINT DefaultCube_Indices[36] = { 0,1,2,1,2,3  ,4,5,6,5,6,7  ,8,9,10,9,10,11  ,12,13,14,13,14,15  ,16,17,18,17,18,19  ,20,21,22,21,22,23 };

	void InitializeAndUploadMesh(Graphics* graphics, bool UseDefaultCube = false);
	Mesh(Graphics* graphics, const std::vector<Vertex_POS_COLOR_UV1_UV2_Normal_Tangent>& Vertices, const std::vector<UINT>& Indices, UINT CountOfVertices, UINT CountOfIndices, CBufferPerObject* CBufferData, bool UseDefaultCube = false, D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
		this->Vertices = Vertices;
		this->Indices = Indices;
		this->CountOfVertices = CountOfVertices;
		this->CountOfIndices = CountOfIndices;
		this->PrimitiveTopology = PrimitiveTopology;
		cbufferPerobj = new ConstantBuffer<CBufferPerObject>(graphics);
		cbufferPerobj->CopyData(CBufferData);
		InitializeAndUploadMesh(graphics, UseDefaultCube);
	}
	Mesh(Graphics* graphics, CBufferPerObject* CBufferData, bool UseDefaultCube = false, D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
		this->PrimitiveTopology = PrimitiveTopology;
		cbufferPerobj = new ConstantBuffer<CBufferPerObject>(graphics);
		cbufferPerobj->CopyData(CBufferData);
		InitializeAndUploadMesh(graphics, UseDefaultCube);
	}
	Mesh(Graphics* graphics, CBufferPerObject* CBufferData,const wchar_t* filepath, D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
		auto model=LoadObjModel(filepath);
		this->Vertices = model.first;
		this->Indices = model.second;
		this->CountOfVertices = model.first.size();
		this->CountOfIndices = model.second.size();
		this->PrimitiveTopology = PrimitiveTopology;
		cbufferPerobj = new ConstantBuffer<CBufferPerObject>(graphics);
		cbufferPerobj->CopyData(CBufferData);
		InitializeAndUploadMesh(graphics, false);
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
	UINT GetCBufferPerObjViewIndex() { return CBufferPerObjectViewIndex; }
	ID3D12Resource* GetCBufferPerObjectForUploadPtr() { return cbufferPerobj->GetCBufferUploadBufferPtr(); }
	UINT GetIndicesCount() { return CountOfIndices; }
	static std::pair<std::vector<Mesh::Vertex_POS_COLOR_UV1_UV2_Normal_Tangent>, std::vector<UINT>> LoadObjModel(const wchar_t* filepath);
	ConstantBuffer<CBufferPerObject>* GetCBufferPerObjPtr() { return cbufferPerobj; }
};
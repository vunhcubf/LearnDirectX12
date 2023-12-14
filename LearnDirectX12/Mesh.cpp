#include "Mesh.h"

void Mesh::InitializeAndUploadMesh(Graphics* graphics,bool UseDefaultCube)
{
	if (VertexBufferGpu) { VertexBufferGpu.Reset(); }
	if (IndexBufferGpu) { IndexBufferGpu.Reset(); }
	//设置几何体
	//创建顶点缓冲视图
	const UINT64 vbByteSize = CountOfVertices * sizeof(Vertex_POS_COLOR_UV1_UV2_Normal_Tangent);
	VertexBufferGpu = graphics->CreateDefaultBuffer(UseDefaultCube? DefaultCube_Vertices: Vertices.data(), vbByteSize, VertexBufferUploaderTemp.Get());

	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = VertexBufferGpu->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(Vertex_POS_COLOR_UV1_UV2_Normal_Tangent);
	vbv.SizeInBytes = vbByteSize;
	VertexBufferView = vbv;
	//创建索引缓冲和视图
	const UINT ibByteSize = CountOfIndices * sizeof(UINT);
	IndexBufferGpu = graphics->CreateDefaultBuffer(UseDefaultCube?DefaultCube_Indices:Indices.data(), ibByteSize, IndexBufferUploaderTemp.Get());

	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = IndexBufferGpu->GetGPUVirtualAddress();
	ibv.Format = DXGI_FORMAT_R32_UINT;
	ibv.SizeInBytes = ibByteSize;
	IndexBufferView = ibv;
}

std::pair<std::vector<Mesh::Vertex_POS_COLOR_UV1_UV2_Normal_Tangent>, std::vector<UINT>> Mesh::LoadObjModel(const wchar_t* filepath)
{
	std::vector<XMFLOAT3> vertices;
	std::vector<XMFLOAT3> normal;
	std::vector<XMFLOAT2> uv;
	std::vector<Mesh::Vertex_POS_COLOR_UV1_UV2_Normal_Tangent> verticesbuffer;
	std::vector<UINT> indicesbuffer;

	std::ifstream ifs;
	ifs.open(filepath);
	if (ifs.is_open()) {
		std::string buffer;
		while (std::getline(ifs, buffer)) {
			//是否为顶点坐标
			if (buffer[0] == 'v') {
				if (buffer[1] == ' ') {
					std::istringstream iss(buffer.substr(2));
					float x, y, z;
					iss >> x >> y >> z;
					vertices.push_back(XMFLOAT3(x, y, z));
				}
				//是否为纹理坐标
				else if (buffer[1] == 't' && buffer[2] == ' ') {
					std::istringstream iss(buffer.substr(3));
					float x, y;
					iss >> x >> y;
					uv.push_back(XMFLOAT2(x, y));
				}
				//是否为法线
				else if (buffer[1] == 'n' && buffer[2] == ' ') {
					std::istringstream iss(buffer.substr(3));
					float x, y, z;
					iss >> x >> y >> z;
					normal.push_back(XMFLOAT3(x, y, z));
				}
			}
			//索引信息
			else if (buffer[0] == 'f' && buffer[1] == ' ') {
				std::string s = buffer.substr(2);
				bool is_v1_vn1 = false;
				int slope_counter = 0;
				for (auto it = s.begin(); it != s.end(); it++) {
					if (*it == '/') {
						slope_counter++;
						*it = ' ';
						if (*(it + 1) == '/' && !is_v1_vn1) {
							is_v1_vn1 = true;
						}
					}
				}
				if (is_v1_vn1) {
					UINT v1, v2, v3, vn1, vn2, vn3;
					std::istringstream iss(s);
					iss >> v1 >> vn1 >> v2 >> vn2 >> v3 >> vn3;
					indicesbuffer.push_back(verticesbuffer.size());
					verticesbuffer.push_back({ vertices[v1 - 1],	XMFLOAT4(0,0,0,0),	XMFLOAT2(0,0),	XMFLOAT2(0,0), normal[vn1 - 1] ,XMFLOAT3(0,0,0) });
					indicesbuffer.push_back(verticesbuffer.size());
					verticesbuffer.push_back({ vertices[v2 - 1],	XMFLOAT4(0,0,0,0),	XMFLOAT2(0,0),	XMFLOAT2(0,0), normal[vn2 - 1] ,XMFLOAT3(0,0,0) });
					indicesbuffer.push_back(verticesbuffer.size());
					verticesbuffer.push_back({ vertices[v3 - 1],	XMFLOAT4(0,0,0,0),	XMFLOAT2(0,0),	XMFLOAT2(0,0), normal[vn3 - 1] ,XMFLOAT3(0,0,0) });
				}
				else if (slope_counter == 6) {
					UINT v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
					std::istringstream iss(s);
					iss >> v1 >> vt1 >> vn1 >> v2 >> vt2 >> vn2 >> v3 >> vt3 >> vn3;
					indicesbuffer.push_back(verticesbuffer.size());
					verticesbuffer.push_back({ vertices[v1 - 1],	XMFLOAT4(0,0,0,0),	uv[vt1 - 1],	XMFLOAT2(0,0), normal[vn1 - 1] ,XMFLOAT3(0,0,0) });
					indicesbuffer.push_back(verticesbuffer.size());
					verticesbuffer.push_back({ vertices[v2 - 1],	XMFLOAT4(0,0,0,0),	uv[vt2 - 1],	XMFLOAT2(0,0), normal[vn2 - 1] ,XMFLOAT3(0,0,0) });
					indicesbuffer.push_back(verticesbuffer.size());
					verticesbuffer.push_back({ vertices[v3 - 1],	XMFLOAT4(0,0,0,0),	uv[vt3 - 1],	XMFLOAT2(0,0), normal[vn3 - 1] ,XMFLOAT3(0,0,0) });
				}
				else if (slope_counter == 3) {
					UINT v1, vt1, v2, vt2, v3, vt3;
					std::istringstream iss(s);
					iss >> v1 >> vt1 >> v2 >> vt2 >> v3 >> vt3;
					indicesbuffer.push_back(verticesbuffer.size());
					verticesbuffer.push_back({ vertices[v1 - 1],	XMFLOAT4(0,0,0,0),	uv[vt1 - 1],	XMFLOAT2(0,0), XMFLOAT3(0,0,0) ,XMFLOAT3(0,0,0) });
					indicesbuffer.push_back(verticesbuffer.size());
					verticesbuffer.push_back({ vertices[v2 - 1],	XMFLOAT4(0,0,0,0),	uv[vt2 - 1],	XMFLOAT2(0,0), XMFLOAT3(0,0,0) ,XMFLOAT3(0,0,0) });
					indicesbuffer.push_back(verticesbuffer.size());
					verticesbuffer.push_back({ vertices[v3 - 1],	XMFLOAT4(0,0,0,0),	uv[vt3 - 1],	XMFLOAT2(0,0), XMFLOAT3(0,0,0) ,XMFLOAT3(0,0,0) });
				}
			}
		}
	}
	ifs.close();
	return { verticesbuffer ,indicesbuffer };
}

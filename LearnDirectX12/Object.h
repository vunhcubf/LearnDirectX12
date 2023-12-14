#pragma once
#include "Mesh.h"
#include "Material.h"

class Object {
public:
	Mesh* mesh;
	Material* material;
	std::vector<Texture*> TextureResourcesCollection;
	ConstantBuffer<Mesh::CBufferPerObject>* cbufferPerObj;
	ConstantBuffer<XMFLOAT4X4>* cbufferPerFrame;
	ConstantBuffer32bits* cbufferPerMaterial;
	Object(Mesh* mesh, Material* material,ConstantBuffer<XMFLOAT4X4>* cbufferPerFrame);
	~Object() {
		TextureResourcesCollection.clear();
		TextureResourcesCollection.shrink_to_fit();
	}
};
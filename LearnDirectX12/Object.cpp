#include "Object.h"

Object::Object(Mesh* mesh,Material* material, ConstantBuffer<XMFLOAT4X4>* cbufferPerFrame)
{
	this->mesh = mesh;
	this->material = material;
	this->TextureResourcesCollection = TextureResourcesCollection;
	this->cbufferPerFrame = cbufferPerFrame;
	this->cbufferPerMaterial = material->cbuffer32bits;
	this->cbufferPerObj = mesh->GetCBufferPerObjPtr();
}

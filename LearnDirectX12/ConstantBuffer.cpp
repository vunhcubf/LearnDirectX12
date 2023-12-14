#pragma once
#include "ConstantBuffer.h"

template <typename T>
ConstantBuffer<T>::ConstantBuffer(Graphics* graphics) {
	UINT CBufferByteSize = CALC_CBUFFER_BYTE_SIZE(sizeof(T));
	THROW_IF_ERROR(graphics->pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(CBufferByteSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(CbufferUploadBuffer.GetAddressOf())));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = CbufferUploadBuffer->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = CBufferByteSize;
	CBufferViewIndex=graphics->AddViewOnCBVHeap(&cbvDesc);

	CbufferUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&CBufferDataPtr));
}

template<typename T>
ConstantBuffer<T>::~ConstantBuffer()
{
	if (CbufferUploadBuffer != nullptr) { CbufferUploadBuffer->Unmap(0, 0); }
	CBufferDataPtr = nullptr;
}

template<typename T>
void ConstantBuffer<T>::CopyData(T* t)
{
	memcpy(CBufferDataPtr, t, sizeof(T));
}
template<typename T>
void ConstantBuffer<T>::CopyData(T t)
{
	memcpy(CBufferDataPtr, &t, sizeof(T));
}
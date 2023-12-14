#pragma once

#include "Graphics.h"

template<typename T>
class ConstantBuffer {
private:
	ComPtr<ID3D12Resource> CbufferUploadBuffer;
	BYTE* CBufferDataPtr=nullptr;
	UINT CBufferViewIndex;
public:
	ID3D12Resource* GetCBufferUploadBufferPtr()const { return CbufferUploadBuffer.Get(); };
	UINT GetCBufferViewIndex()const { return CBufferViewIndex; };
	ConstantBuffer(Graphics* graphics);
	~ConstantBuffer();
	void CopyData(T* t);
	void CopyData(T t);
};

class ConstantBuffer32bits {
private:
	ComPtr<ID3D12Resource> CbufferUploadBuffer;
	BYTE* CBufferDataPtr = nullptr;
	UINT CBufferViewIndex;
	UINT SizeInByte;
public:
	ID3D12Resource* GetCBufferUploadBufferPtr()const { return CbufferUploadBuffer.Get(); };
	UINT GetCBufferViewIndex()const { return CBufferViewIndex; };
	ConstantBuffer32bits(Graphics* graphics, UINT SizeInByte) {
		this->SizeInByte = SizeInByte;
		UINT CBufferByteSize = CALC_CBUFFER_BYTE_SIZE(SizeInByte);
		THROW_IF_ERROR(graphics->pID3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(CBufferByteSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(CbufferUploadBuffer.GetAddressOf())));

		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = CbufferUploadBuffer->GetGPUVirtualAddress();
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = CBufferByteSize;
		this->CBufferViewIndex = graphics->AddViewOnCBVHeap(&cbvDesc);

		CbufferUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&CBufferDataPtr));
	}
	~ConstantBuffer32bits() {
		if (CbufferUploadBuffer != nullptr) { CbufferUploadBuffer->Unmap(0, 0); }
		CBufferDataPtr = nullptr;
	}
	void CopyData(void* t) {
		memcpy(CBufferDataPtr, t, SizeInByte);
	}
};
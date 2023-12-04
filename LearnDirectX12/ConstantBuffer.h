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
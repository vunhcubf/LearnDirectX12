#include "Renderer.h"

void Renderer::SetRenderTarget(UINT CameraColorTargetIndex, UINT CameraDepthStencilTargetIndex, ID3D12Resource* ColorResource, ID3D12Resource* DepthStencilResource)
{
	this->CameraColorTargetIndex = CameraColorTargetIndex;
	this->CameraDepthStencilTargetIndex = CameraDepthStencilTargetIndex;
	this->ColorResource = ColorResource;
	this->DepthStencilResource = DepthStencilResource;
}

void Renderer::ClearRenderTarget()
{
	WillClearRenderTarget = true;
}

void Renderer::DrawMesh(Material* mat, Mesh* mesh)
{
	//����ȾĿ������ΪĿ��״̬
	graphics->pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ColorResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	if (WillClearRenderTarget) {
		//�����ȾĿ������ģ�建��
		graphics->pCommandList->ClearRenderTargetView(graphics->FetchIndexedViewCpuHandleFromCBV_SRV_UAVHeap(&CameraColorTargetIndex), Colors::LightSteelBlue, 0, nullptr);
		graphics->pCommandList->ClearDepthStencilView(graphics->FetchIndexedViewCpuHandleFromDSVHeap(&CameraDepthStencilTargetIndex), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}
	//������ȾĿ��
	graphics->pCommandList->OMSetRenderTargets(1, &graphics->CurrentBackBufferView(), true, &graphics->DepthStencilView());
	//���ó���������������
	ID3D12DescriptorHeap* descriptorHeaps[] = { graphics->CBV_SRV_UAVHeap.Get() };
	graphics->pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//���ø�ǩ��
	graphics->pCommandList->SetGraphicsRootSignature(mat->GetRootSignaturePtr());
	//�󶨶������������
	graphics->pCommandList->IASetVertexBuffers(0, 1, mesh->GetVertexBufferViewPtr());
	graphics->pCommandList->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());
	graphics->pCommandList->IASetIndexBuffer(mesh->GetIndexBufferViewPtr());
}

#include "App.h"

void App::ProcessMessage() {
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			this->QuitAppCounter += 1;
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}
MyWindow* App::GetWindows() const
{
	return windows;
}
void App::Quit() {
	delete windows;
	delete eventManager;

	delete graphics;
	delete mesh;
	delete mat;
	delete CBufferPerFrame;
	delete renderer;
}
void App::Initlize() {
	eventManager = new WindowsEventManager();
	camera = Camera(1280, 720);
	windows = MyWindow::RegisterForm(hInstance, 1280, 720, L"Hello World!", L"Hello World!");
	windows->SetWindowEventHandlePtr(eventManager);

	//注册Camera的事件
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::KEYDOWN, (void*)&camera, Camera::InvokeEvent_KEYDOWN);
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::KEYUP, (void*)&camera, Camera::InvokeEvent_KEYUP);
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::RBUTTONDOWN, (void*)&camera, Camera::InvokeEvent_RBUTTONDOWN);
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::RBUTTONUP, (void*)&camera, Camera::InvokeEvent_RBUTTONUP);
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::MOUSEMOVE, (void*)&camera, Camera::InvokeEvent_MOUSEMOVE);

	ByteCodeVS = Graphics::CompileShader(L"D:\\LearnDirectX12\\LearnDirectX12\\Shaders\\DrawCube.hlsl", nullptr, "VS", "vs_5_0");
	ByteCodePS = Graphics::CompileShader(L"D:\\LearnDirectX12\\LearnDirectX12\\Shaders\\DrawCube.hlsl", nullptr, "PS", "ps_5_0");

	MyWindow::ShowForm(windows);

	graphics = new Graphics(windows);
	renderer = new Renderer(graphics);

	mesh = new Mesh(graphics, &Mesh::CBufferPerObject(graphics->Float4x4Identity, XMFLOAT4(0,0,0,0)));

	mat = new Material(Graphics::GetShaderByteCodeFromBlob(ByteCodeVS.Get()), Graphics::GetShaderByteCodeFromBlob(ByteCodePS.Get()), 1);
	
	mesh->InitializeAndUploadMesh(graphics, true);
	CBufferPerFrame = new ConstantBuffer<XMFLOAT4X4>(graphics);
}
void App::DoFrame() {
	//更新相机的VP矩阵
	camera.OnUpdate((int)timer.mDeltaTime.count());
	camera.CalcCameraVPMatrix();
	CBufferPerFrame->CopyData(Graphics::MatrixToFloat4x4(XMMatrixTranspose(camera.CameraVPMatrix)));

	renderer->SetRenderTarget(graphics->mSwapChainBufferIndex[graphics->CurrentBackBufferIndex], graphics->mDepthStencilBufferIndex, graphics->mSwapChainBuffer[graphics->CurrentBackBufferIndex].Get(), graphics->mDepthStencilBuffer.Get());
	renderer->ClearRenderTarget();

	mat->SetConstantBuffer(mesh->GetCBufferPerObjViewIndex(), mesh->GetCBufferPerObjectForUploadPtr());
	mat->SetConstantBuffer(CBufferPerFrame);

	renderer->DrawMesh(mat,mesh);
	renderer->ExcuteCommandListWithBlockWithResetSwapChain();
}
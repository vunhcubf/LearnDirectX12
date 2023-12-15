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

	delete GBufferB;
	delete GBufferA;

	delete graphics;
	delete CBufferPerFrame;
	delete renderer;

	for (auto it = MeshCollection.begin(); it != MeshCollection.end(); it++) {
		delete it->second;
	}
	for (auto it = MaterialCollection.begin(); it != MaterialCollection.end(); it++) {
		delete it->second;
	}
	MeshCollection.clear();
	MaterialCollection.clear();
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
	StatusConsole.PrintLine(L"注册活动相机事件\n");
	graphics = new Graphics(windows, &StatusConsole);
	renderer = new Renderer(graphics);

	//遍历shader文件夹中的所有文件
	std::filesystem::path directoryPath(L"D:\\LearnDirectX12\\LearnDirectX12\\Shaders");
	bool IsAnyError = false;
	if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
		for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
			std::wstring path = entry.path().wstring();
			std::wstring filename = entry.path().filename().wstring();
			ShaderVSCollection.insert({ filename, graphics->CompileShader(&IsAnyError,path, nullptr, "VS", "vs_5_0") });
			ShaderPSCollection.insert({ filename, graphics->CompileShader(&IsAnyError,path, nullptr, "PS", "ps_5_0") });
		}
	}
	if (IsAnyError) { Sleep(INFINITE); }
	//导入材质
	TextureCollection[L"默认"] = new Texture(L"D:\\LearnDirectX12\\LearnDirectX12\\Textures\\chicken.dds", graphics);
	StatusConsole.PrintLine(std::wstring(L"导入了贴图资源:")+std::wstring(L"D:\\LearnDirectX12\\LearnDirectX12\\Textures\\chicken.dds"+std::wstring(L"\n")));
	
	//生成逐帧的Cbuffer
	CBufferPerFrame = new ConstantBuffer<XMFLOAT4X4>(graphics);
	//生成所有的材质
	MaterialResources resources1;
	resources1.AddTexture(TextureCollection[L"默认"]);
	resources1.AddFloat4(XMFLOAT4(1, 0, 1, 1));
	resources1.AddFloat4(XMFLOAT4(1, 1, 0, 1));
	resources1.AddFloat4(XMFLOAT4(0.5, 0.5, 0.5, 0.5));
	MaterialCollection[L"cornel_outer"] = new Material(Graphics::GetShaderByteCodeFromBlob(ShaderVSCollection[L"DrawCube.hlsl"].Get()), Graphics::GetShaderByteCodeFromBlob(ShaderPSCollection[L"DrawCube.hlsl"].Get()), graphics, resources1, 1);
	MaterialResources resources2;
	resources2.AddTexture(TextureCollection[L"默认"]);
	resources2.AddFloat4(XMFLOAT4(1, 0, 1, 1));
	resources2.AddFloat4(XMFLOAT4(0, 0, 1, 1));
	resources2.AddFloat4(XMFLOAT4(0.5, 0.5, 0.5, 0.5));
	MaterialCollection[L"cornel_sphere"] = new Material(Graphics::GetShaderByteCodeFromBlob(ShaderVSCollection[L"DrawCube.hlsl"].Get()), Graphics::GetShaderByteCodeFromBlob(ShaderPSCollection[L"DrawCube.hlsl"].Get()), graphics, resources2, 1);
	MaterialResources resources3;
	resources3.AddTexture(TextureCollection[L"默认"]);
	resources3.AddFloat4(XMFLOAT4(1, 0, 1, 1));
	resources3.AddFloat4(XMFLOAT4(0, 1, 0, 1));
	resources3.AddFloat4(XMFLOAT4(0.5, 0.5, 0.5, 0.5));
	MaterialCollection[L"cornel_monkey"] = new Material(Graphics::GetShaderByteCodeFromBlob(ShaderVSCollection[L"DrawCube.hlsl"].Get()), Graphics::GetShaderByteCodeFromBlob(ShaderPSCollection[L"DrawCube.hlsl"].Get()), graphics, resources3, 1);
	MaterialResources resources4;
	resources4.AddTexture(TextureCollection[L"默认"]);
	resources4.AddFloat4(XMFLOAT4(1, 0, 1, 1));
	resources4.AddFloat4(XMFLOAT4(1, 0, 0, 1));
	resources4.AddFloat4(XMFLOAT4(0.5, 0.5, 0.5, 0.5));
	MaterialCollection[L"cornel_box"] = new Material(Graphics::GetShaderByteCodeFromBlob(ShaderVSCollection[L"DrawCube.hlsl"].Get()), Graphics::GetShaderByteCodeFromBlob(ShaderPSCollection[L"DrawCube.hlsl"].Get()), graphics, resources4, 1);

	//生成所有的网格
	MeshCollection[L"cornel_sphere"] = new Mesh(graphics, &Mesh::CBufferPerObject(graphics->Float4x4Identity, XMFLOAT4(0, 0, 0, 0)), L"D:\\LearnDirectX12\\LearnDirectX12\\Models\\cornel_sphere.obj");
	MeshCollection[L"cornel_outer"] = new Mesh(graphics, &Mesh::CBufferPerObject(graphics->Float4x4Identity, XMFLOAT4(0, 0, 0, 0)), L"D:\\LearnDirectX12\\LearnDirectX12\\Models\\cornel_outer.obj");
	MeshCollection[L"cornel_monkey"] = new Mesh(graphics, &Mesh::CBufferPerObject(graphics->Float4x4Identity, XMFLOAT4(0, 0, 0, 0)), L"D:\\LearnDirectX12\\LearnDirectX12\\Models\\cornel_monkey.obj");
	MeshCollection[L"cornel_box"] = new Mesh(graphics, &Mesh::CBufferPerObject(graphics->Float4x4Identity, XMFLOAT4(0, 0, 0, 0)), L"D:\\LearnDirectX12\\LearnDirectX12\\Models\\cornel_box.obj");

	//生成所有的物体
	ObjectCollection[L"cornel_outer"] = new Object(MeshCollection[L"cornel_outer"], MaterialCollection[L"cornel_outer"], CBufferPerFrame);
	ObjectCollection[L"cornel_sphere"] = new Object(MeshCollection[L"cornel_sphere"], MaterialCollection[L"cornel_sphere"], CBufferPerFrame);
	ObjectCollection[L"cornel_monkey"] = new Object(MeshCollection[L"cornel_monkey"], MaterialCollection[L"cornel_monkey"], CBufferPerFrame);
	ObjectCollection[L"cornel_box"] = new Object(MeshCollection[L"cornel_box"], MaterialCollection[L"cornel_box"], CBufferPerFrame);
	//GBufferA
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc_gbuffera;
	srv_desc_gbuffera.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc_gbuffera.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc_gbuffera.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc_gbuffera.Texture2D.MostDetailedMip = 0;
	srv_desc_gbuffera.Texture2D.MipLevels = 1;
	srv_desc_gbuffera.Texture2D.ResourceMinLODClamp = 0;
	srv_desc_gbuffera.Texture2D.PlaneSlice = 0;
	D3D12_RENDER_TARGET_VIEW_DESC rtv_desc_gbuffera;
	rtv_desc_gbuffera.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtv_desc_gbuffera.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtv_desc_gbuffera.Texture2D.MipSlice = 0;
	rtv_desc_gbuffera.Texture2D.PlaneSlice = 0;
	GBufferA = new Texture(&rtv_desc_gbuffera, &srv_desc_gbuffera, CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, windows->Width, windows->Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET), graphics);
	//GBUfferB
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc_gbufferb;
	srv_desc_gbufferb.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc_gbufferb.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc_gbufferb.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc_gbufferb.Texture2D.MostDetailedMip = 0;
	srv_desc_gbufferb.Texture2D.MipLevels = 1;
	srv_desc_gbufferb.Texture2D.ResourceMinLODClamp = 0;
	srv_desc_gbufferb.Texture2D.PlaneSlice = 0;
	D3D12_RENDER_TARGET_VIEW_DESC rtv_desc_gbufferb;
	rtv_desc_gbufferb.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtv_desc_gbufferb.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtv_desc_gbufferb.Texture2D.MipSlice = 0;
	rtv_desc_gbufferb.Texture2D.PlaneSlice = 0;
	GBufferB = new Texture(&rtv_desc_gbufferb,&srv_desc_gbufferb,CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, windows->Width, windows->Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),graphics);
	
	MyWindow::ShowForm(windows);
}
void App::DoFrame() {
	//更新相机的VP矩阵
	camera.OnUpdate((int)timer.mDeltaTime.count());
	camera.CalcCameraVPMatrix();
	renderer->ClearRenderTarget();
	CBufferPerFrame->CopyData(Graphics::MatrixToFloat4x4(XMMatrixTranspose(camera.CameraVPMatrix)));

	//渲染gbuffer
	std::vector<Texture*> gbuffer_targets;
	gbuffer_targets.push_back(GBufferA);
	gbuffer_targets.push_back(GBufferB);
	renderer->SetRenderTarget(gbuffer_targets,2,graphics->GetSwapchainDSBufferIndex(), graphics->GetSwapchainDSBuffer());
	renderer->DrawRendererMrt(ObjectCollection);


	renderer->Blit(GBufferB);
	renderer->ExcuteCommandList();
	renderer->RefreshSwapChain();
	renderer->WaitGpu();
	renderer->ClearCommandAllocater();
	renderer->ClearCommandList();
	//必须先重置命令分配器，再清空命令队列
	//不重置命令分配器会发生内存泄漏
}
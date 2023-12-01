#include "App.h"

void App::Initlize() {
	eventManager = new WindowsEventManager();
	camera = Camera(1280,720);
	windows = MyWindow::RegisterForm(hInstance, 1280, 720, L"Hello World!", L"Hello World!");
	windows->SetWindowEventHandlePtr(eventManager);

	//×¢²áCameraµÄÊÂ¼þ
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::KEYDOWN,(void*)&camera,Camera::InvokeEvent_KEYDOWN);
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::KEYUP, (void*)&camera, Camera::InvokeEvent_KEYUP);
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::RBUTTONDOWN, (void*)&camera, Camera::InvokeEvent_RBUTTONDOWN);
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::RBUTTONUP, (void*)&camera, Camera::InvokeEvent_RBUTTONUP);
	windows->GetWindowEventHandlePtr()->RegisterEvent(EventType::MOUSEMOVE, (void*)&camera, Camera::InvokeEvent_MOUSEMOVE);

	MyWindow::ShowForm(windows);
	graphics = new Graphics(windows);
	graphics->InitBox();
}
void App::ProcessMessage() {
	MSG msg;
	while (PeekMessage(&msg,nullptr,0,0,PM_REMOVE)) {
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
	this->graphics->QuitBox();
	delete windows;
	delete graphics;
	delete eventManager;
}
void App::DoFrame() {
	camera.OnUpdate((int)timer.mDeltaTime.count());
	graphics->Update(&camera);
	graphics->DrawBox();
}
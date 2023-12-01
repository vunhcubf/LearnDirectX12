#include "App.h"

void App::Initlize() {
	camera = Camera(800,600);
	windows = MyWindow::RegisterForm(hInstance, 800, 600, L"LearnDx12", L"Main");
	windows->camera = &camera;
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
}
void App::DoFrame() {
	//this->graphics.at(0)->DrawEmpty();
	camera.OnUpdate((int)timer.mDeltaTime.count());
	graphics->Update(&camera);
	graphics->DrawBox();
}
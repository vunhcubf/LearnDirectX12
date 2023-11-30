#include "App.h"

void App::Initlize() {
	this->windows = std::vector<MyWindow*>(1);
	this->windows.at(0) = MyWindow::RegisterForm(hInstance, 1280, 720, L"LearnDx12", L"Main");
	MyWindow::ShowForm(this->windows.at(0));
	this->graphics = std::vector<Graphics*>(1);
	this->graphics.at(0) = new Graphics(this->windows.at(0));
	this->graphics.at(0)->InitBox();
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
std::vector<MyWindow*> App::GetWindows() const
{
	return windows;
}
void App::Quit() {
	this->graphics.at(0)->QuitBox();
	for (MyWindow*& e : this->windows) {
		delete e;
	}
	for (Graphics*& e : this->graphics) {
		delete e;
	}
}
void App::DoFrame() {
	//this->graphics.at(0)->DrawEmpty();
	this->graphics.at(0)->DrawBox();
}
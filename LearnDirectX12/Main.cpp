#include "App.h"
#include <chrono>

int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int mCmdShow) {
	try {
		App* app = new App(hInstance, hPrevInstance, lpCmdLine,mCmdShow);
		app->Initlize();
		GameTimer timer;
		while (true) {
			app->ProcessMessage();
			if (app->QuitAppCounter == app->GetWindowsCount()) {
				break;
			}
			app->DoFrame();

			timer.Tick();
			timer.DisplayFps(app->GetWindows().at(0));
			//timer.DisplayFps(app->GetWindows().at(1));
			timer.SetPrevTime();
		}
		app->Quit();
	}
	catch (const MyException& e) {
		MessageBoxExA(nullptr,e.what(),e.GetType(),MB_OK | MB_ICONEXCLAMATION, 0x0409);
	}
	catch (const std::exception& e) {
		MessageBoxExA(nullptr,e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION, 0x0409);
	}
	catch (...) {
		MessageBoxExA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION, 0x0409);
	}
	return -1;
}
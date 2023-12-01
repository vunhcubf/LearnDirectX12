#include "App.h"
#include <chrono>

int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int mCmdShow) {
	try {
		App* app = new App(hInstance, hPrevInstance, lpCmdLine,mCmdShow);
		app->Initlize();
		while (true) {
			app->ProcessMessage();
			if (app->QuitAppCounter == 1) {
				break;
			}
			app->DoFrame();

			app->timer.Tick();
			app->timer.SetPrevTime();
			app->timer.DisplayFps(app->GetWindows());
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
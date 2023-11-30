#include "GameTimer.h"
#include <stdio.h>

GameTimer::GameTimer()
{
	mPrevTime = std::chrono::steady_clock::now();
}

void GameTimer::Tick()
{
	mCurTime = std::chrono::steady_clock::now();
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(mCurTime - mPrevTime);
	if (duration < MinFrameTime) {
		std::this_thread::sleep_for(MinFrameTime - duration);
		mDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(mCurTime - mPrevTime+ MinFrameTime - duration);
	}
	else {
		mDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(mCurTime - mPrevTime);
	}
}

void GameTimer::DisplayFps(MyWindow* Wnd)
{
	std::wstring ws;
	ws = Wnd->WindowTitle;
	ws += L"  Fps:";
	ws += std::to_wstring(1000/mDeltaTime.count());
	SetWindowTextW(Wnd->hWnd, ws.c_str());
}

void GameTimer::SetPrevTime()
{
	mPrevTime= std::chrono::steady_clock::now();
}

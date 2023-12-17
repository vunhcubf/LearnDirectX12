#include "GameTimer.h"
#include <stdio.h>
#include <thread>

GameTimer::GameTimer()
{
	mPrevTime = std::chrono::steady_clock::now();
}

void GameTimer::Tick()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	mCurTime = std::chrono::steady_clock::now();
	mDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(mCurTime - mPrevTime);
	mTotalTime += mDeltaTime;
}

void GameTimer::DisplayFps(MyWindow* Wnd,std::wstring GpuInfo,UINT GpuId)
{
	std::wstringstream ws;
	ws << Wnd->WindowTitle;
	ws << L"  Fps:";
	ws << std::to_wstring(1000/mDeltaTime.count());
	ws << L"    GpuId:";
	ws << GpuId;
	ws<< L"  ";
	ws << GpuInfo;
	SetWindowTextW(Wnd->hWnd, ws.str().c_str());
}

void GameTimer::SetPrevTime()
{
	mPrevTime= std::chrono::steady_clock::now();
}

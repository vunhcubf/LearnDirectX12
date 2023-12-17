#pragma once
#include "MyWindow.h"
#include <string>
#include <chrono>
class GameTimer
{
public:
    GameTimer();
    void Tick();  // 每帧都要调用
    void DisplayFps(MyWindow* Wnd,std::wstring GpuInfo,UINT GpuId);
    void SetPrevTime();

    std::chrono::milliseconds mTotalTime = std::chrono::duration<long long, std::milli>(0);
    std::chrono::milliseconds mDeltaTime= std::chrono::duration<long long, std::milli>(0);
    std::chrono::steady_clock::time_point mCurTime= std::chrono::steady_clock::now();;
    std::chrono::steady_clock::time_point mPrevTime= std::chrono::steady_clock::now();;
    static constexpr int MaxFps = 60;
    static constexpr std::chrono::milliseconds MinFrameTime = std::chrono::milliseconds(long(1000.0 / double(MaxFps)));
};
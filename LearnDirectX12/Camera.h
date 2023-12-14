#pragma once
#include <DirectXMath.h>
#include <utility>
#include "MyWindow.h"

using namespace DirectX;

#ifndef DEFINED_CAMERA
#define DEFINED_CAMERA

class Camera {
public:
	std::pair<int, int> MousePos;
	XMVECTOR WorldPos = XMVectorSet(3.63156, 3.24489, 3.83895, 1);
	float FoVH = 45;
	float Aspect = 1;
	float NearClipPlane = 0.1;
	float FarClipPlane = 1000;
	static constexpr float Deg2Rad = 57.2957795131f;
	XMMATRIX CameraPMatrix;
	XMMATRIX CameraVMatrix;
	XMMATRIX CameraInvVMatrix;
	XMMATRIX CameraVPMatrix;
	float mPhi = 1.0355;
	float mTheta = 0.756677;
	void CalcCameraPMatrix();
	void CalcCameraVMatrix();
	void CalcCameraVPMatrix();

	void OnUpdate(int deltatime);
	Camera(int width, int height);
	Camera() {}
	//事件管理相关
	static void InvokeEvent_KEYDOWN(void* InstPtr, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		((Camera*)InstPtr)->On_KEYDOWN(hWnd, msg, wParam, lParam);
	}
	static void InvokeEvent_MOUSEMOVE(void* InstPtr, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		((Camera*)InstPtr)->On_MOUSEMOVE(hWnd, msg, wParam, lParam);
	}
	static void InvokeEvent_KEYUP(void* InstPtr, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		((Camera*)InstPtr)->On_KEYUP(hWnd, msg, wParam, lParam);
	}
	static void InvokeEvent_RBUTTONDOWN(void* InstPtr, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		((Camera*)InstPtr)->On_RBUTTONDOWN(hWnd, msg, wParam, lParam);
	}
	static void InvokeEvent_RBUTTONUP(void* InstPtr, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		((Camera*)InstPtr)->On_RBUTTONUP(hWnd, msg, wParam, lParam);
	}

	void On_KEYDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void On_KEYUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void On_RBUTTONDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void On_RBUTTONUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void On_MOUSEMOVE(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//事件状态相关
	bool IsRButtonDown = false;
	bool IsPressedW = false;
	bool IsPressedS = false;
	bool IsPressedA = false;
	bool IsPressedD = false;
	bool IsPressedQ = false;
	bool IsPressedE = false;
	bool IsInitialized = false;
};
#endif
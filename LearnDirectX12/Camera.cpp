#include "Camera.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

void Camera::CalcCameraPMatrix()
{
	this->CameraPMatrix = XMMatrixPerspectiveFovLH(this->FoVH / Deg2Rad, Aspect, NearClipPlane, FarClipPlane);
}

void Camera::CalcCameraVMatrix()
{
	float x = -sinf(mPhi) * cosf(mTheta);
	float z = -sinf(mPhi) * sinf(mTheta);
	float y = -cosf(mPhi);

	XMVECTOR pos = WorldPos;
	XMVECTOR target = WorldPos + XMVectorSet(x, y, z, 0);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	CameraVMatrix = XMMatrixLookAtLH(pos, target, up);
	CameraInvVMatrix = XMMatrixInverse(nullptr, CameraVMatrix);
}

void Camera::CalcCameraVPMatrix()
{
	CameraVPMatrix = CameraVMatrix * CameraPMatrix;
}

void Camera::OnUpdate(int deltatime)
{
	if ((IsPressedW || IsPressedS || IsPressedA || IsPressedD || IsPressedQ || IsPressedE) && IsRButtonDown) {
		int DirectionWS = IsPressedW - IsPressedS;
		int DirectionAD = IsPressedD - IsPressedA;
		int DirectionQE = IsPressedE - IsPressedQ;
		double speed = 1e-2;
		XMVECTOR Movement = XMVectorSet(DirectionAD, DirectionQE, DirectionWS, 1);
		XMFLOAT4X4 Tmp;
		XMStoreFloat4x4(&Tmp, CameraInvVMatrix);
		XMMATRIX CameraInvVMatrixVector = XMMATRIX(
			Tmp._11, Tmp._12, Tmp._13, 0,
			Tmp._21, Tmp._22, Tmp._23, 0,
			Tmp._31, Tmp._32, Tmp._33, 0,
			0, 0, 0, 1
		);
		XMVECTOR MovementW = XMVector4Transform(Movement * speed * deltatime, CameraInvVMatrixVector);
		MovementW *= XMVectorSet(1, 1, 1, 0);
		WorldPos += MovementW;
	}
	CalcCameraPMatrix();
	CalcCameraVMatrix();
	CalcCameraVPMatrix();
}

Camera::Camera(int width, int height)
{
	Aspect = double(width) / double(height);
}

void Camera::On_KEYDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	unsigned char pressedKey = unsigned char(wParam);
	switch (pressedKey)
	{
	case('W'):
		IsPressedW = true;
		break;
	case('S'):
		IsPressedS = true;
		break;

	case('A'):
		IsPressedA = true;
		break;
	case('D'):
		IsPressedD = true;
		break;

	case('Q'):
		IsPressedQ = true;
		break;
	case('E'):
		IsPressedE = true;
		break;
	}
}

void Camera::On_KEYUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	unsigned char pressedKey = unsigned char(wParam);
	switch (pressedKey)
	{
	case('W'):
		IsPressedW = false;
		break;
	case('S'):
		IsPressedS = false;
		break;

	case('A'):
		IsPressedA = false;
		break;
	case('D'):
		IsPressedD = false;
		break;

	case('Q'):
		IsPressedQ = false;
		break;
	case('E'):
		IsPressedE = false;
		break;
	}
}

void Camera::On_RBUTTONDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { IsRButtonDown = true; }
void Camera::On_RBUTTONUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { IsRButtonDown = false; }

void Camera::On_MOUSEMOVE(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT clientRect;
	GetClientRect(hWnd,&clientRect);
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hWnd, &mousePos);
	bool isMouseInside = PtInRect(&clientRect, mousePos);
	if (!isMouseInside) {
		IsRButtonDown = false;
	}

	const POINTS pt = MAKEPOINTS(lParam);
	int x = pt.x;
	int y = pt.y;
	int dx, dy;
	if (IsInitialized) {
		dx = x - MousePos.first;
		dy = y - MousePos.second;
	}
	else {
		dx = 0;
		dy = 0;
		IsInitialized = true;
	}
	MousePos.first = x;
	MousePos.second = y;
	if (!IsRButtonDown) { return; }
	mTheta -= dx * 0.2 / Deg2Rad;
	mPhi -= dy * 0.2 / Deg2Rad;
	mPhi = max(min(mPhi, 3.14159265358979f*0.95f), 3.14159265358979f * 0.05f);
	mTheta = mTheta > 3.14159265358979f * 2.0f ? mTheta - 3.14159265358979f * 2.0f : mTheta;
}

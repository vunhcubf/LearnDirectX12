#include "Camera.h"
#include <algorithm>

void Camera::CalcCameraPMatrix()
{
	this->CameraPMatrix = XMMatrixPerspectiveFovLH(this->FoVH / Deg2Rad, Aspect, NearClipPlane, FarClipPlane);
}

void Camera::CalcCameraVMatrix()
{
	float x =-sinf(mPhi) * cosf(mTheta);
	float z =-sinf(mPhi) * sinf(mTheta);
	float y =-cosf(mPhi);

	XMVECTOR pos = WorldPos;
	XMVECTOR target = WorldPos+ XMVectorSet(x,y,z,0);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	CameraVMatrix = XMMatrixLookAtLH(pos, target, up);
	CameraInvVMatrix = XMMatrixInverse(nullptr, CameraVMatrix);
}

void Camera::CalcCameraVPMatrix()
{
	CameraVPMatrix = CameraVMatrix * CameraPMatrix;
}

void Camera::OnRightButtonDown()
{
	this->IsRightPressed = true;
}

void Camera::OnRightButtonUp()
{
	this->IsRightPressed = false;
}

void Camera::OnMouseMove(int x, int y)
{
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
	if (!IsRightPressed) { return; }
	mTheta -= dx * 0.15 / Deg2Rad;
	mPhi -= dy * 0.15 / Deg2Rad;
	mPhi = std::clamp(mPhi,0.1f,3.0f);
	mTheta = std::clamp(mTheta, 0.1f, 6.2f);
}

void Camera::OnWPressed()
{
	IsWPressed = true;
}

void Camera::OnWReleased()
{
	IsWPressed = false;
}

void Camera::OnUpdate(int deltatime)
{
	double speed = 1e-2;
	if (IsRightPressed && IsWPressed) {
		XMVECTOR Movement = XMVectorSet(speed * deltatime,0,0, 1);
		XMVECTOR MovementW = XMVector4Transform(Movement, XMMatrixTranspose(CameraInvVMatrix));
		MovementW *= XMVectorSet(1,1,1,0);
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

#pragma once
#include <DirectXMath.h>
#include <utility>
using namespace DirectX;

class Camera {
public:
	bool IsInitialized = false;
	bool IsRightPressed = false;
	bool IsWPressed = false;
	std::pair<int, int> MousePos;
	XMVECTOR WorldPos= XMVectorSet(2,2,2,1);
	float FoVH=45;
	float Aspect=1;
	float NearClipPlane=1;
	float FarClipPlane=1000;
	static constexpr float Deg2Rad = 57.2957795131f;
	XMMATRIX CameraPMatrix;
	XMMATRIX CameraVMatrix;
	XMMATRIX CameraInvVMatrix;
	XMMATRIX CameraVPMatrix;
	float mPhi=0.1;
	float mTheta=3.14;
	void CalcCameraPMatrix();
	void CalcCameraVMatrix();
	void CalcCameraVPMatrix();

	void OnRightButtonDown();
	void OnRightButtonUp();
	void OnMouseMove(int x, int y);
	void OnWPressed();
	void OnWReleased();

	void OnUpdate(int deltatime);
	Camera(int width, int height);
	Camera() {
		CameraPMatrix = XMMATRIX(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		CameraVMatrix = XMMATRIX(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		CameraVPMatrix = XMMATRIX(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
	}
};
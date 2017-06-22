#ifndef CAMERA_H
#define CAMERA_H

#include "ClassIncludes.h"
#include "Controls.h"

#include <windows.h>



class Camera
{
private:
	float xPos;
	float yPos;
	float zPos;
	float angle;

	XMFLOAT2 lastMousePos;
	XMFLOAT2 lookDegrees;
	XMFLOAT3 lookPoint;

	D3D11_MAPPED_SUBRESOURCE shaderBufferPointer;
	wvpBuffer *BufferPointer;
	ID3D11Buffer* WVP;
	ID3D11Buffer* cameraPosition;

	XMFLOAT4X4 worldM;
	XMFLOAT4X4 viewM;
	XMFLOAT4X4 projectionM;

public:
	Camera(ID3D11Device* gDevice, HWND window);
	virtual ~Camera();

	float getXPos();
	float getZPos();
	float getAngle();

	ID3D11Buffer** getConstBuffer();

	XMFLOAT4X4* getViewMatrix();
	XMFLOAT4X4* getProjectionMatrix();

	void SetCamera(ID3D11DeviceContext* gDeviceContext, XMFLOAT3 origin, XMFLOAT3 lookAt);
	void UpdateCamera(ID3D11DeviceContext* gDeviceContext, XMFLOAT3 origin, Controls* gamepad);

	float getTurnX();
	float getTurnY();

};

#endif
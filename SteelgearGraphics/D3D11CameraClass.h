#ifndef D3D11_CAMERA_CLASS_H
#define D3D11_CAMERA_CLASS_H

#include "EngineIncludes.h"
#include "D3D11Includes.h"

struct Entity;
class TransformHandler;

struct ViewProjectionData
{
	XMFLOAT4X4 viewM;
	XMFLOAT4X4 projectionM;
};

struct D3DCameraData
{
	XMFLOAT4X4 viewM;
	XMFLOAT4X4 projectionM;
	ID3D11Buffer* bufferViewProjection = nullptr;
	ID3D11Buffer* bufferCameraPosition = nullptr; // for shadow maps if I remember correctly?
	int transformID;

};

class D3D11CameraClass
{
private:
	std::vector<D3DCameraData> cameras;
	std::vector<int> freeSpots;

	TransformHandler* transformHandler;

	int activeCamera = -1;

public:
	D3D11CameraClass();
	virtual ~D3D11CameraClass();
	void SetTransformHandler(TransformHandler* handler);

	int CreateCamera(float fov, float aspectRatio, float nearPlane, float farPlane, bool setActive, ID3D11Device* device, int transformID);

	void UpdateActiveCamera(ID3D11DeviceContext* deviceContext);

	ID3D11Buffer** GetActiveViewProjectionBuffer();
	ID3D11Buffer** GetActiveCameraPositionBuffer();

};

#endif
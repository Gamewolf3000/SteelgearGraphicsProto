#include "D3D11CameraClass.h"
#include "EntityHandler.h"

D3D11CameraClass::D3D11CameraClass()
{
}

D3D11CameraClass::~D3D11CameraClass()
{
	for (int i = 0; i < cameras.size(); i++)
	{
		SafeReleaseD3D(cameras[i].bufferViewProjection);
		SafeReleaseD3D(cameras[i].bufferCameraPosition);
	}
}

void D3D11CameraClass::SetTransformHandler(TransformHandler * handler)
{
	transformHandler = handler;
}

int D3D11CameraClass::CreateCamera(float fov, float aspectRatio, float nearPlane, float farPlane, bool setActive, ID3D11Device* device, int transformID)
{
	int returnID = -1;

	if (freeSpots.size() > 0)
	{
		returnID = freeSpots[freeSpots.size() - 1];
		cameras[transformID].transformID = transformID;

		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
	}
	else
	{
		D3DCameraData temp;
		temp.transformID = transformID;
		cameras.push_back(temp);
		returnID = cameras.size() - 1;
	}

	if (setActive)
	{
		activeCamera = returnID;
	}

	XMStoreFloat4x4(&cameras[returnID].projectionM, XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane));

	D3D11_BUFFER_DESC bufferDescConstBuffer;
	memset(&bufferDescConstBuffer, 0, sizeof(bufferDescConstBuffer));
	bufferDescConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescConstBuffer.ByteWidth = sizeof(cameras[returnID].viewM) + sizeof(cameras[returnID].projectionM);
	bufferDescConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bufferDescConstBuffer, nullptr, &cameras[returnID].bufferViewProjection);

	//---------------------------------------------------------------------------------------------------------------------------------------------------------

	D3D11_BUFFER_DESC bufferDescConstBuffer2;
	memset(&bufferDescConstBuffer2, 0, sizeof(bufferDescConstBuffer2));
	bufferDescConstBuffer2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescConstBuffer2.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescConstBuffer2.ByteWidth = sizeof(XMFLOAT4);
	bufferDescConstBuffer2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	device->CreateBuffer(&bufferDescConstBuffer2, nullptr, &cameras[returnID].bufferCameraPosition);

	return returnID;
}

void D3D11CameraClass::UpdateActiveCamera(ID3D11DeviceContext* deviceContext)
{
	HRESULT hr;
	//TransformData data = transformHandler->transforms[cameras[activeCamera].transformID];

	//XMMATRIX tempMV = XMMatrixLookAtLH(data.position, data.direction, data.up);
	XMMATRIX tempMV = XMMatrixLookAtLH(XMVectorSet(-80.0f, 90.0f, -120.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&cameras[activeCamera].viewM, (tempMV));
	tempMV = XMMatrixTranspose(tempMV);

	XMMATRIX tempMP = XMMatrixTranspose(XMLoadFloat4x4(&cameras[activeCamera].projectionM));

	D3D11_MAPPED_SUBRESOURCE shaderBufferPointer;
	CameraStruct* BufferPointer;

	hr = deviceContext->Map(cameras[activeCamera].bufferViewProjection, 0, D3D11_MAP_WRITE_DISCARD, 0, &shaderBufferPointer);

	BufferPointer = (CameraStruct*)shaderBufferPointer.pData;

	XMStoreFloat4x4(&BufferPointer->viewM, tempMV);
	XMStoreFloat4x4(&BufferPointer->projectionM, tempMP);

	deviceContext->Unmap(cameras[activeCamera].bufferViewProjection, 0);



	D3D11_MAPPED_SUBRESOURCE resource;

	hr = deviceContext->Map(cameras[activeCamera].bufferCameraPosition, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	XMFLOAT4* BufferPointer2 = (XMFLOAT4*)resource.pData;
	//XMStoreFloat4(BufferPointer2, data.position);
	XMStoreFloat4(BufferPointer2, XMVectorSet(0, 0, -12, 1));

	deviceContext->Unmap(cameras[activeCamera].bufferCameraPosition, 0);
}

ID3D11Buffer ** D3D11CameraClass::GetActiveViewProjectionBuffer()
{
	return &cameras[activeCamera].bufferViewProjection;
}

ID3D11Buffer ** D3D11CameraClass::GetActiveCameraPositionBuffer()
{
	return &cameras[activeCamera].bufferCameraPosition;
}

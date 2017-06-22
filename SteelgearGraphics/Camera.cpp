#include "Camera.h"

Camera::Camera(ID3D11Device* gDevice, HWND window)
{
	//set the starting values

	xPos = 0;
	yPos = 0;
	zPos = 0;
	angle = 1.57079632679489661923132169163975144209858469968755291048747229615390820314310449931401741267105853399107404325664115332f; //PI divided by 2

	XMStoreFloat4x4(&worldM, XMMatrixIdentity());
	XMStoreFloat4x4(&viewM, XMMatrixLookAtLH(XMVectorSet(0.0f, 1.0f, -6.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&projectionM, XMMatrixPerspectiveFovLH(XM_PI*0.45, (WIDTH / HEIGHT), 0.1f, 25.0f));

	D3D11_BUFFER_DESC bufferDescConstBuffer;
	memset(&bufferDescConstBuffer, 0, sizeof(bufferDescConstBuffer));
	bufferDescConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescConstBuffer.ByteWidth = sizeof(wvpBuffer);
	bufferDescConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	gDevice->CreateBuffer(&bufferDescConstBuffer, nullptr, &WVP);

	//---------------------------------------------------------------------------------------------------------------------------------------------------------

	D3D11_BUFFER_DESC bufferDescConstBuffer2;
	memset(&bufferDescConstBuffer2, 0, sizeof(bufferDescConstBuffer2));
	bufferDescConstBuffer2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescConstBuffer2.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescConstBuffer2.ByteWidth = sizeof(XMFLOAT4);
	bufferDescConstBuffer2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &XMFLOAT4(xPos, yPos, zPos, 0);

	gDevice->CreateBuffer(&bufferDescConstBuffer2, &data, &cameraPosition);

	ShowCursor(false);
	RECT screen;
	GetWindowRect(window, &screen);
	lookDegrees.x = lookDegrees.y = 0;
	
	SetCursorPos((screen.right - screen.left) / 2 + screen.left, (screen.bottom - screen.top) / 2 + screen.top);
	lastMousePos.x = (screen.right - screen.left) / 2 + screen.left;
	lastMousePos.y = (screen.bottom - screen.top) / 2 + screen.top;

}

Camera::~Camera()
{
	SafeReleaseD3D(WVP);
	SafeReleaseD3D(cameraPosition);
}

float Camera::getXPos()
{
	return xPos;
}

float Camera::getZPos()
{
	return zPos;
}

float Camera::getAngle()
{
	return angle;
}

ID3D11Buffer** Camera::getConstBuffer()
{
	return &WVP;
}

XMFLOAT4X4* Camera::getViewMatrix()
{
	return &viewM;
}

XMFLOAT4X4* Camera::getProjectionMatrix()
{
	return &projectionM;
}

void Camera::SetCamera(ID3D11DeviceContext* gDeviceContext, XMFLOAT3 origin, XMFLOAT3 lookAt)
{

	XMMATRIX tempMV = XMMatrixLookAtLH(XMVectorSet(origin.x, origin.y, origin.z, 0.0f), XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&viewM, (tempMV));
	tempMV = XMMatrixTranspose(tempMV);

	XMMATRIX tempMP = XMMatrixTranspose(XMLoadFloat4x4(&projectionM));



	gDeviceContext->Map(WVP, 0, D3D11_MAP_WRITE_DISCARD, 0, &shaderBufferPointer);

	BufferPointer = (wvpBuffer*)shaderBufferPointer.pData;

	XMStoreFloat4x4(&BufferPointer->world, XMLoadFloat4x4(&worldM));
	XMStoreFloat4x4(&BufferPointer->view, tempMV);
	XMStoreFloat4x4(&BufferPointer->projection, tempMP);

	gDeviceContext->Unmap(WVP, 0);


	D3D11_MAPPED_SUBRESOURCE resource;

	gDeviceContext->Map(cameraPosition, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	float4Buffer* BufferPointer = (float4Buffer*)resource.pData;
	BufferPointer->data = XMFLOAT4(xPos, yPos, zPos, 0);

	gDeviceContext->Unmap(cameraPosition, 0);

	gDeviceContext->VSSetConstantBuffers(2, 1, &cameraPosition);
	gDeviceContext->GSSetConstantBuffers(2, 1, &cameraPosition);
}

void Camera::UpdateCamera(ID3D11DeviceContext* gDeviceContext, XMFLOAT3 origin, Controls* gamepad)
{

	RECT screen;
	POINT p;

	GetCursorPos(&p);
	GetWindowRect(GetActiveWindow(), &screen);

	XMVECTOR lookAtVec = XMVectorSet(0, 0, 1, 0);

	lookDegrees.x += ((p.x - lastMousePos.x) / (WIDTH / 2)) * XM_PIDIV4;
	lookDegrees.y += ((p.y - lastMousePos.y) / (HEIGHT / 2)) * XM_PIDIV4;

	lookDegrees.x += ((gamepad->GetXBoxController()->GetRightStickValue('X') / 2) / (WIDTH / 2)) * XM_PIDIV4;
	lookDegrees.y += (-(gamepad->GetXBoxController()->GetRightStickValue('Y') / 4) / (HEIGHT / 2)) * XM_PIDIV4;

	if (lookDegrees.x > XM_2PI || lookDegrees.x < -XM_2PI)
	{
		lookDegrees.x = 0;
	}

	if (lookDegrees.y > XM_PIDIV2 * 0.9)
	{
		lookDegrees.y = XM_PIDIV2 *0.9;
	}
	else if (lookDegrees.y < -XM_PIDIV2 * 0.9)
	{
		lookDegrees.y = -XM_PIDIV2 * 0.9;
	}

	lookAtVec = XMVector3Transform(lookAtVec, XMMatrixRotationX(lookDegrees.y));
	lookAtVec = XMVector3Transform(lookAtVec, XMMatrixRotationY(lookDegrees.x));
	lookAtVec = XMVector3Transform(lookAtVec, XMMatrixTranslation(origin.x, origin.y, origin.z));

	lastMousePos.x = p.x;
	lastMousePos.y = p.y;

	if (p.x < screen.left)
	{
		SetCursorPos((screen.right - screen.left) / 2 + screen.left, p.y);
		lastMousePos.x = (screen.right - screen.left) / 2 + screen.left;
		SetCamera(gDeviceContext, origin, XMFLOAT3(XMVectorGetX(lookAtVec), XMVectorGetY(lookAtVec), XMVectorGetZ(lookAtVec)));;
	}
	else if (p.x > screen.right)
	{
		SetCursorPos((screen.right - screen.left) / 2 + screen.left, p.y);
		lastMousePos.x = (screen.right - screen.left) / 2 + screen.left;
		SetCamera(gDeviceContext, origin, XMFLOAT3(XMVectorGetX(lookAtVec), XMVectorGetY(lookAtVec), XMVectorGetZ(lookAtVec)));;
	}

	if (p.y < screen.top)
	{
		SetCursorPos(p.x, (screen.bottom - screen.top) / 2 + screen.top);
		lastMousePos.y = (screen.bottom - screen.top) / 2 + screen.top;
		SetCamera(gDeviceContext, origin, XMFLOAT3(XMVectorGetX(lookAtVec), XMVectorGetY(lookAtVec), XMVectorGetZ(lookAtVec)));;
	}
	else if (p.y > screen.bottom)
	{
		SetCursorPos(p.x, (screen.bottom - screen.top) / 2 + screen.top);
		lastMousePos.y = (screen.bottom - screen.top) / 2 + screen.top;
		SetCamera(gDeviceContext, origin, XMFLOAT3(XMVectorGetX(lookAtVec), XMVectorGetY(lookAtVec), XMVectorGetZ(lookAtVec)));;
	}

	lookPoint = XMFLOAT3(XMVectorGetX(lookAtVec), XMVectorGetY(lookAtVec), XMVectorGetZ(lookAtVec));

	SetCamera(gDeviceContext, origin, XMFLOAT3(XMVectorGetX(lookAtVec), XMVectorGetY(lookAtVec), XMVectorGetZ(lookAtVec)));

}

float Camera::getTurnX()
{
	return lookDegrees.x;
}

float Camera::getTurnY()
{
	return lookDegrees.y;
}
#include "D3D11BaseClass.h"
#include "EntityHandler.h"
#include "CameraHandler.h"

D3D11BaseClass::D3D11BaseClass(HINSTANCE hInstance, int width, int height, wchar_t directory[200])
{
	wndHandle = InitWindow(hInstance, width, height);

	CreateDirect3DContext(wndHandle, width, height);

	SetViewport(width, height);

	//--------------------------------

	InitialiseStates();
	//SetShadowResources();

	//--------------------------------

	shaders = new D3D11ShaderClass(directory);
	buffers = new D3D11BufferClass;
	materials = new D3D11MaterialClass(directory);

	InitiateConstBuffers();

	EnvironmentMapData defaultEnvMap;
	defaultEnvMap.typeOfMap = staticMap;

	BuildCubeMapViews(defaultEnvMap);


	float clearColor[] = { 0, 0, 0, 1 };

	for (int i = 0; i < 6; i++)
	{
		deviceContext->ClearRenderTargetView(cubeMaps[0].rtvs[i], clearColor);
		//deviceContext->ClearDepthStencilView(cubeMaps[0].dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
	}

	deviceContext->GenerateMips(cubeMaps[0].srv);

}

D3D11BaseClass::~D3D11BaseClass()
{
	delete shaders;
	delete buffers;
	delete materials;

	SafeReleaseD3D(bufferWorld);
	SafeReleaseD3D(bufferViewProj);
	SafeReleaseD3D(bufferCameraPosition);
	SafeReleaseD3D(bufferLight);
	SafeReleaseD3D(bufferCameraData);

	SafeReleaseD3D(backbufferRTV);
	SafeReleaseD3D(swapChain);
	SafeReleaseD3D(device);
	SafeReleaseD3D(deviceContext);

	SafeReleaseD3D(depthStencilBuffer);
	SafeReleaseD3D(depthStencilView);

	for (int i = 0; i < cubeMaps.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			SafeReleaseD3D(cubeMaps[i].rtvs[j]);
			SafeReleaseD3D(irradianceMaps[i].rtvs[j]);
		}

		delete cubeMaps[i].rtvs;
		delete irradianceMaps[i].rtvs;

		SafeReleaseD3D(cubeMaps[i].dsv);
		SafeReleaseD3D(cubeMaps[i].srv);
		SafeReleaseD3D(irradianceMaps[i].dsv);
		SafeReleaseD3D(irradianceMaps[i].srv);
	}

	//SafeReleaseD3D(sDepthStencilBuffer);
	//SafeReleaseD3D(sDepthStencilView);
	//SafeReleaseD3D(shadowRTV);
	//SafeReleaseD3D(shadowResourceView);
	//SafeReleaseD3D(shadowRaState);
	//SafeReleaseD3D(shadowStencilState);

	DestroyWindow(wndHandle);

	SafeReleaseD3D(wrapSample);
	SafeReleaseD3D(sampleShadow);
	SafeReleaseD3D(transparency);
	SafeReleaseD3D(onState);
	SafeReleaseD3D(offState);
}

HWND D3D11BaseClass::InitWindow(HINSTANCE hInstance, int width, int height)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &D3D11BaseClass::WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"JOCHENGINEEX";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, width, height};
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"JOCHENGINEEX",
		L"JochEngineEX",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK D3D11BaseClass::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT D3D11BaseClass::CreateDirect3DContext(HWND wndHandle, int width, int height)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapChain,
		&device,
		NULL,
		&deviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		device->CreateRenderTargetView(pBackBuffer, NULL, &backbufferRTV);


		pBackBuffer->Release();

		D3D11_TEXTURE2D_DESC depthStencilDesc; // Create DepthStencil
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer);
		device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView);

		// set the render target as the back buffer
		deviceContext->OMSetRenderTargets(1, &backbufferRTV, depthStencilView);
	}
	return hr;
}

void D3D11BaseClass::SetViewport(int width, int height)
{
	//D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	//svp.Width = SHADOWRES;
	//svp.Height = SHADOWRES;
	//svp.MinDepth = 0.0f;
	//svp.MaxDepth = 1.0f;
	//svp.TopLeftX = 0;
	//svp.TopLeftY = 0;
}

void D3D11BaseClass::InitiateConstBuffers()
{
	D3D11_BUFFER_DESC constDesc;
	memset(&constDesc, 0, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.Usage = D3D11_USAGE_DYNAMIC;
	constDesc.ByteWidth = sizeof(XMFLOAT4X4);
	constDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&constDesc, nullptr, &bufferWorld);

	//---------------------------------------------------------------------------------------------------------------------------------------------------------

	D3D11_BUFFER_DESC bufferDescConstBuffer;
	memset(&bufferDescConstBuffer, 0, sizeof(bufferDescConstBuffer));
	bufferDescConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescConstBuffer.ByteWidth = sizeof(ViewProjStruct);
	bufferDescConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bufferDescConstBuffer, nullptr, &bufferViewProj);

	//---------------------------------------------------------------------------------------------------------------------------------------------------------

	D3D11_BUFFER_DESC bufferDescConstBuffer2;
	memset(&bufferDescConstBuffer2, 0, sizeof(bufferDescConstBuffer2));
	bufferDescConstBuffer2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescConstBuffer2.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescConstBuffer2.ByteWidth = sizeof(XMFLOAT4);
	bufferDescConstBuffer2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	device->CreateBuffer(&bufferDescConstBuffer2, nullptr, &bufferCameraPosition);

	//---------------------------------------------------------------------------------------------------------------------------------------------------------

	D3D11_BUFFER_DESC bufferDescConstBuffer3;
	memset(&bufferDescConstBuffer3, 0, sizeof(bufferDescConstBuffer3));
	bufferDescConstBuffer3.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescConstBuffer3.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescConstBuffer3.ByteWidth = sizeof(XMFLOAT4) * 256 * 3;
	bufferDescConstBuffer3.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bufferDescConstBuffer3, nullptr, &bufferLight);

	//---------------------------------------------------------------------------------------------------------------------------------------------------------

	D3D11_BUFFER_DESC bufferDescConstBuffer4;
	memset(&bufferDescConstBuffer4, 0, sizeof(bufferDescConstBuffer4));
	bufferDescConstBuffer4.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescConstBuffer4.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescConstBuffer4.ByteWidth = sizeof(CameraDataStruct);
	bufferDescConstBuffer4.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bufferDescConstBuffer4, nullptr, &bufferCameraData);
}

HWND D3D11BaseClass::GetWindow()
{
	return wndHandle;
}

void D3D11BaseClass::SwapBackBuffer()
{
	swapChain->Present(0, 0);
}

void D3D11BaseClass::InitialiseStates()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &wrapSample);

	//deviceContext->VSSetSamplers(0, 1, &wrapSample);
	deviceContext->PSSetSamplers(0, 1, &wrapSample);

	//--------------------------------------------------------------------------------------------

	D3D11_SAMPLER_DESC samplerDesc2;
	ZeroMemory(&samplerDesc2, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc2.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;//D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;//D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;//D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;//D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc2.MipLODBias = 0.0f;
	samplerDesc2.MaxAnisotropy = 1;
	samplerDesc2.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_ALWAYS;
	samplerDesc2.BorderColor[0] = 0;
	samplerDesc2.BorderColor[1] = 0;
	samplerDesc2.BorderColor[2] = 0;
	samplerDesc2.BorderColor[3] = 0;
	samplerDesc2.MinLOD = 0;
	samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc2, &sampleShadow);

	deviceContext->PSSetSamplers(1, 1, &sampleShadow);

	//--------------------------------------------------------------------------------------------

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_BLEND_DESC transparentDesc;
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&transparentDesc, &transparency);

	deviceContext->OMSetBlendState(transparency, NULL, 0xffffffff);

	//----------------------------------------------------------------------------------------------

	D3D11_RASTERIZER_DESC onDesc;
	onDesc.FillMode = D3D11_FILL_SOLID;
	onDesc.CullMode = D3D11_CULL_BACK;
	onDesc.FrontCounterClockwise = false;
	onDesc.DepthBias = 0;
	onDesc.SlopeScaledDepthBias = 0.0f;
	onDesc.DepthBiasClamp = 0.0f;
	onDesc.DepthClipEnable = false;
	onDesc.ScissorEnable = false;
	onDesc.MultisampleEnable = false;
	onDesc.AntialiasedLineEnable = false;

	device->CreateRasterizerState(&onDesc, &onState);

	D3D11_RASTERIZER_DESC offDesc;
	offDesc.FillMode = D3D11_FILL_SOLID;
	offDesc.CullMode = D3D11_CULL_NONE;
	offDesc.FrontCounterClockwise = false;
	offDesc.DepthBias = 0;
	offDesc.SlopeScaledDepthBias = 0.0f;
	offDesc.DepthBiasClamp = 0.0f;
	offDesc.DepthClipEnable = false;
	offDesc.ScissorEnable = false;
	offDesc.MultisampleEnable = false;
	offDesc.AntialiasedLineEnable = false;

	device->CreateRasterizerState(&offDesc, &offState);

	//D3D11_RASTERIZER_DESC sRasDesc;
	//offDesc.FillMode = D3D11_FILL_SOLID;
	//offDesc.CullMode = D3D11_CULL_NONE;
	//offDesc.FrontCounterClockwise = false;
	//offDesc.DepthBias = 0;
	//offDesc.SlopeScaledDepthBias = 0.0f;
	//offDesc.DepthBiasClamp = 0.0f;
	//offDesc.DepthClipEnable = true;
	//offDesc.ScissorEnable = false;
	//offDesc.MultisampleEnable = false;
	//offDesc.AntialiasedLineEnable = false;

	//device->CreateRasterizerState(&offDesc, &shadowRaState);

	//----------------------------------------------------------------------------------------------

	//D3D11_DEPTH_STENCIL_DESC depthStateDesc;
	//depthStateDesc.DepthEnable = true;
	//depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//depthStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	//depthStateDesc.StencilEnable = false;
	//depthStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	//depthStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	//depthStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//depthStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//depthStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	//depthStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	//depthStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//depthStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//depthStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//depthStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	//device->CreateDepthStencilState(&depthStateDesc, &shadowStencilState);

}

void D3D11BaseClass::SetShadowResources()
{
	//HRESULT shadow;

	//D3D11_TEXTURE2D_DESC descDepth;
	//descDepth.Width = SHADOWRES;
	//descDepth.Height = SHADOWRES;
	//descDepth.MipLevels = 1;
	//descDepth.ArraySize = 1;
	//descDepth.Format = DXGI_FORMAT_R32_TYPELESS; //DXGI_FORMAT_R32_FLOAT;
	//descDepth.SampleDesc.Count = 1;
	//descDepth.SampleDesc.Quality = 0;
	//descDepth.Usage = D3D11_USAGE_DEFAULT;
	//descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	//descDepth.CPUAccessFlags = 0;
	//descDepth.MiscFlags = 0;


	//ID3D11Texture2D *pDepthBuffer = NULL;
	//shadow = gDevice->CreateTexture2D(&descDepth, NULL, &pDepthBuffer);

	//D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	//ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	//descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	//descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//descDSV.Texture2D.MipSlice = 0;

	//shadow = gDevice->CreateDepthStencilView(pDepthBuffer, &descDSV, &sDepthStencilView);

	//D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	//srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	//srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srDesc.Texture2D.MostDetailedMip = 0;
	//srDesc.Texture2D.MipLevels = 1;

	//shadow = gDevice->CreateShaderResourceView(pDepthBuffer, &srDesc, &shadowResourceView);

	//pDepthBuffer->Release();

}

void D3D11BaseClass::SetBackFaceCull(bool on)
{
	if (on == true)
		deviceContext->RSSetState(onState);
	else
		deviceContext->RSSetState(offState);

}

//void D3D11BaseClass::RenderShadows()
//{
//	gDeviceContext->OMSetBlendState(nullBlend, NULL, 0xffffffff);
//	gDeviceContext->RSSetState(shadowRaState);
//	gDeviceContext->OMSetDepthStencilState(shadowStencilState, 0);
//
//	gDeviceContext->PSSetShaderResources(13, 1, &nullResourceView);
//	gDeviceContext->OMSetRenderTargets(1, &nullRTV, sDepthStencilView);
//
//	gDeviceContext->RSSetViewports(1, &svp);
//	gDeviceContext->ClearDepthStencilView(sDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
//
//}

void D3D11BaseClass::PreRender()
{
	SetStandardSamplers();

	deviceContext->OMSetBlendState(transparency, NULL, 0xffffffff);
	deviceContext->RSSetState(nullptr);
	deviceContext->OMSetDepthStencilState(nullptr, 0);

	deviceContext->OMSetRenderTargets(1, &backbufferRTV, depthStencilView);

	float clearColor[] = { 1, 0, 1, 1 };
	deviceContext->RSSetViewports(1, &vp);
	deviceContext->ClearRenderTargetView(backbufferRTV, clearColor);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);

	deviceContext->VSSetConstantBuffers(1, 1, &bufferViewProj);
	deviceContext->GSSetConstantBuffers(1, 1, &bufferViewProj);
	//deviceContext->VSSetConstantBuffers(2, 1, cameras->GetActiveCameraPositionBuffer());
	//deviceContext->GSSetConstantBuffers(2, 1, cameras->GetActiveCameraPositionBuffer());

	//deviceContext->PSSetShaderResources(13, 1, &shadowResourceView);


	deviceContext->PSSetConstantBuffers(1, 1, &bufferCameraData);

}

void D3D11BaseClass::PreCubeMapRender(unsigned int cubeMapID, unsigned int rtvNR, bool isIrradianceMap)
{
	SetStandardSamplers();

	deviceContext->OMSetBlendState(transparency, NULL, 0xffffffff);
	deviceContext->RSSetState(nullptr);
	deviceContext->OMSetDepthStencilState(nullptr, 0);

	if (isIrradianceMap)
	{
		deviceContext->OMSetRenderTargets(1, &irradianceMaps[cubeMapID].rtvs[rtvNR], irradianceMaps[cubeMapID].dsv);

		float clearColor[] = { 0, 1, 0, 1 };
		deviceContext->RSSetViewports(1, &irradianceMaps[cubeMapID].viewPort);
		deviceContext->ClearRenderTargetView(irradianceMaps[cubeMapID].rtvs[rtvNR], clearColor);
		deviceContext->ClearDepthStencilView(irradianceMaps[cubeMapID].dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
	}
	else
	{

		deviceContext->OMSetRenderTargets(1, &cubeMaps[cubeMapID].rtvs[rtvNR], cubeMaps[cubeMapID].dsv);

		float clearColor[] = { 1, 0, 1, 1 };
		deviceContext->RSSetViewports(1, &cubeMaps[cubeMapID].viewPort);
		deviceContext->ClearRenderTargetView(cubeMaps[cubeMapID].rtvs[rtvNR], clearColor);
		deviceContext->ClearDepthStencilView(cubeMaps[cubeMapID].dsv, D3D11_CLEAR_DEPTH, 1.0, 0);

	}

	deviceContext->VSSetConstantBuffers(1, 1, &bufferViewProj);
	deviceContext->GSSetConstantBuffers(1, 1, &bufferViewProj);

	//deviceContext->PSSetConstantBuffers(1, 1, &bufferCameraData);

}

void D3D11BaseClass::SetMeshBuffers(int meshID)
{
	UINT32 vertexSize = sizeof(StandardVertex);
	UINT32 offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, buffers->GetVertexBuffer(meshID), &vertexSize, &offset);
	deviceContext->IASetIndexBuffer(buffers->GetIndexBuffer(meshID), DXGI_FORMAT_R32_UINT, offset);
}

void D3D11BaseClass::SetShaders(int shaderID)
{
	shaders->SetShaders(shaderID, deviceContext);
}

void D3D11BaseClass::SetTransformBuffer(Float4x4 transform)
{
	D3D11_MAPPED_SUBRESOURCE resource;

	deviceContext->Map(bufferWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	XMFLOAT4X4* bufferPointer = (XMFLOAT4X4*)resource.pData;
	
	XMStoreFloat4x4(bufferPointer, XMMatrixTranspose(XMLoadFloat4x4(&transform)));

	deviceContext->Unmap(bufferWorld, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &bufferWorld);
	deviceContext->GSSetConstantBuffers(0, 1, &bufferWorld);
}

void D3D11BaseClass::SetCameraBuffers(CameraData data)
{
	HRESULT hr;

	XMMATRIX tempMV;
	tempMV = XMMatrixTranspose(XMLoadFloat4x4(&data.viewM));

	XMMATRIX tempMP = XMMatrixTranspose(XMLoadFloat4x4(&data.projectionM));

	D3D11_MAPPED_SUBRESOURCE shaderBufferPointer;
	ViewProjStruct* BufferPointer;

	hr = deviceContext->Map(bufferViewProj, 0, D3D11_MAP_WRITE_DISCARD, 0, &shaderBufferPointer);

	BufferPointer = (ViewProjStruct*)shaderBufferPointer.pData;

	XMStoreFloat4x4(&BufferPointer->viewM, tempMV);
	XMStoreFloat4x4(&BufferPointer->projectionM, tempMP);

	deviceContext->Unmap(bufferViewProj, 0);




	D3D11_MAPPED_SUBRESOURCE shaderBufferPointer2;
	CameraDataStruct* BufferPointer2;

	hr = deviceContext->Map(bufferCameraData, 0, D3D11_MAP_WRITE_DISCARD, 0, &shaderBufferPointer2);

	BufferPointer2 = (CameraDataStruct*)shaderBufferPointer2.pData;

	BufferPointer2->position = XMFLOAT4(data.position.x + data.offset.x, data.position.y + data.offset.y, data.position.z + data.offset.z, 1.0f);
	BufferPointer2->lookAt = XMFLOAT4(data.lookAt.x, data.lookAt.y, data.lookAt.z, 0.0f);

	deviceContext->Unmap(bufferCameraData, 0);



	//D3D11_MAPPED_SUBRESOURCE resource;

	//hr = deviceContext->Map(cameras[activeCamera].bufferCameraPosition, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	//XMFLOAT4* BufferPointer2 = (XMFLOAT4*)resource.pData;
	////XMStoreFloat4(BufferPointer2, data.position);
	//XMStoreFloat4(BufferPointer2, XMVectorSet(0, 0, -12, 1));

	//deviceContext->Unmap(cameras[activeCamera].bufferCameraPosition, 0);
}

void D3D11BaseClass::SetViewProj(CameraData data)
{
	HRESULT hr;

	XMMATRIX tempMV;
	tempMV = XMMatrixTranspose(XMLoadFloat4x4(&data.viewM));

	XMMATRIX tempMP = XMMatrixTranspose(XMLoadFloat4x4(&data.projectionM));
	//XMMATRIX invProj = XMMatrixInverse(nullptr, tempMP);
	//XMMATRIX test = tempMP * invProj;

	D3D11_MAPPED_SUBRESOURCE shaderBufferPointer;
	ViewProjStruct* BufferPointer;

	hr = deviceContext->Map(bufferViewProj, 0, D3D11_MAP_WRITE_DISCARD, 0, &shaderBufferPointer);

	BufferPointer = (ViewProjStruct*)shaderBufferPointer.pData;

	XMStoreFloat4x4(&BufferPointer->viewM, tempMV);
	XMStoreFloat4x4(&BufferPointer->projectionM, tempMP);

	deviceContext->Unmap(bufferViewProj, 0);
}

void D3D11BaseClass::SetMaterials(EntityMaterials data)
{
	deviceContext->PSSetShaderResources(0, 1, materials->GetMaterial(data.diffuseTex));
	deviceContext->PSSetShaderResources(1, 1, materials->GetMaterial(data.roughnessTex));
	deviceContext->PSSetShaderResources(2, 1, materials->GetMaterial(data.metallicTex));
	deviceContext->PSSetShaderResources(3, 1, materials->GetMaterial(data.normalMap));
	deviceContext->PSSetShaderResources(4, 1, materials->GetMaterial(data.displacementMap));
	//deviceContext->PSSetShaderResources(1, 1, &sprites[(int)sprite]);
	//deviceContext->PSSetShaderResources(2, 1, &sprites[(int)sprite]);

	//deviceContext->PSSetShaderResources(3, 1, &sprites[(int)sprite]);
	//deviceContext->PSSetShaderResources(4, 1, &sprites[(int)sprite]);

}

void D3D11BaseClass::SetSingleMaterial(unsigned int materialID, MaterialType typeOfMaterial)
{
	if (typeOfMaterial == diffuse)
	{
		deviceContext->PSSetShaderResources(0, 1, materials->GetMaterial(materialID));
	}
	else if (typeOfMaterial == roughness)
	{
		deviceContext->PSSetShaderResources(1, 1, materials->GetMaterial(materialID));
	}
	else if (typeOfMaterial == metallic)
	{
		deviceContext->PSSetShaderResources(2, 1, materials->GetMaterial(materialID));
	}
	else if (typeOfMaterial == normal)
	{
		deviceContext->PSSetShaderResources(3, 1, materials->GetMaterial(materialID));
	}
	else if (typeOfMaterial == displacement)
	{
		deviceContext->PSSetShaderResources(4, 1, materials->GetMaterial(materialID));
	}
}

void D3D11BaseClass::SetCubeMap(int cubeMapID)
{
	if (cubeMapID < 0)
	{
		deviceContext->PSSetShaderResources(5, 1, &cubeMaps[0].srv);
		deviceContext->PSSetShaderResources(6, 1, &irradianceMaps[0].srv);
	}
	else
	{
		deviceContext->GenerateMips(cubeMaps[cubeMapID].srv);
		deviceContext->PSSetShaderResources(5, 1, &cubeMaps[cubeMapID].srv);
		deviceContext->GenerateMips(irradianceMaps[cubeMapID].srv);
		deviceContext->PSSetShaderResources(6, 1, &irradianceMaps[cubeMapID].srv);
	}
}

void D3D11BaseClass::SetTopology(D3D_PRIMITIVE_TOPOLOGY topology)
{
	deviceContext->IASetPrimitiveTopology(topology);
}

void D3D11BaseClass::SetLightBuffers(std::vector<LightData>& lights, std::vector<Float3D> &positions)
{

	LightStruct bufferPointer;// = (LightStruct*)resource.pData;

	//bufferPointer.positions = new XMFLOAT4[256];
	//bufferPointer.colours = new XMFLOAT4[256];
	//bufferPointer.lightRangesTypes = new XMFLOAT4[256];

	for (int i = 0; i < lights.size(); i++)
	{
		bufferPointer.positions[i] = XMFLOAT4(positions[i].x + lights[i].offset.x, positions[i].y + lights[i].offset.y, positions[i].z + lights[i].offset.z, 1.0f);
		bufferPointer.colours[i] = XMFLOAT4(lights[i].colour.x, lights[i].colour.y, lights[i].colour.z, 1.0f);
		bufferPointer.lightRangesTypes[i].x = lights[i].range;
		bufferPointer.lightRangesTypes[i].y = lights[i].typeOfLight;
	}

	for (int i = lights.size(); i < 256; i++)
	{
		bufferPointer.positions[i] = XMFLOAT4(0, 0, 0, 1.0f);
		bufferPointer.colours[i] = XMFLOAT4(0, 0, 0, 1.0f);
		bufferPointer.lightRangesTypes[i].x = 0;
		bufferPointer.lightRangesTypes[i].y = -1;
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(resource));

	deviceContext->Map(bufferLight, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);


	memcpy(resource.pData, &bufferPointer, sizeof(XMFLOAT4) * 256 * 3);
	deviceContext->Unmap(bufferLight, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &bufferLight);

	//delete bufferPointer.positions;
	//delete bufferPointer.colours;
	//delete bufferPointer.lightRangesTypes;
}

void D3D11BaseClass::Render(int nrOfIndices)
{
	deviceContext->DrawIndexed(nrOfIndices, 0, 0);
}

void D3D11BaseClass::BuildCubeMapViews(EnvironmentMapData& entityData)
{

	//http://richardssoftware.net/Home/Post/26
	//http://richardssoftware.net/Home/Post/25
	//http://www.codinglabs.net/article_physically_based_rendering_cook_torrance.aspx
	//http://www.codinglabs.net/article_physically_based_rendering.aspx

	HRESULT hr;
	CubeMapData tempStruct, irradianceStruct;
	ZeroMemory(&tempStruct, sizeof(tempStruct));
	ZeroMemory(&irradianceStruct, sizeof(irradianceStruct));

	// create the render target cube map texture
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = cubeMapSize;
	texDesc.Height = cubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = NULL; //??? Acceptable?
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* cubeTex = nullptr;
	ID3D11Texture2D* irradianceTex = nullptr;
	ZeroMemory(&cubeTex, sizeof(cubeTex));
	ZeroMemory(&cubeTex, sizeof(irradianceTex));
	hr = device->CreateTexture2D(&texDesc, NULL, &cubeTex);
	hr = device->CreateTexture2D(&texDesc, NULL, &irradianceTex);

	// create the render target view array
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	ID3D11RenderTargetView** tempRTV = new ID3D11RenderTargetView*[6];
	ID3D11RenderTargetView** irradianceRTV = new ID3D11RenderTargetView*[6];

	for (int i = 0; i < 6; i++) {
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		hr = device->CreateRenderTargetView(cubeTex, &rtvDesc, &tempRTV[i]);
		hr = device->CreateRenderTargetView(irradianceTex, &rtvDesc, &irradianceRTV[i]);
		//_dynamicCubeMapRTV[i] = new RenderTargetView(Device, cubeTex, rtvDesc);
	}
	tempStruct.rtvs = tempRTV;
	irradianceStruct.rtvs = irradianceRTV;

	// Create the shader resource view that we will bind to our effect for the cubemap
	D3D11_SHADER_RESOURCE_VIEW_DESC  srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	ID3D11ShaderResourceView* tempSRV;
	ID3D11ShaderResourceView* irradianceSRV;
	hr = device->CreateShaderResourceView(cubeTex, &srvDesc, &tempSRV);
	hr = device->CreateShaderResourceView(irradianceTex, &srvDesc, &irradianceSRV);
	//_dynamicCubeMapSRV = new ShaderResourceView(Device, cubeTex, srvDesc);
	tempStruct.srv = tempSRV;
	irradianceStruct.srv = irradianceSRV;

	// release the texture, now that it is saved to the views
	SafeReleaseD3D(cubeTex);
	SafeReleaseD3D(irradianceTex);
	//Util.ReleaseCom(ref cubeTex);

	// create the depth/stencil texture
	D3D11_TEXTURE2D_DESC depthTexDesc;
	ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
	depthTexDesc.Width = cubeMapSize;
	depthTexDesc.Height = cubeMapSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	//depthTexDesc.SampleDescription = new SampleDescription(1, 0);
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = NULL; //??? Acceptable?
	depthTexDesc.MiscFlags = NULL; //??? Acceptable?

	ID3D11Texture2D* depthTex;// = new Texture2D(Device, depthTexDesc);
	ID3D11Texture2D* irradianceDepthTex;
	ZeroMemory(&depthTex, sizeof(depthTex));
	ZeroMemory(&depthTex, sizeof(irradianceDepthTex));
	hr = device->CreateTexture2D(&depthTexDesc, NULL, &depthTex);
	hr = device->CreateTexture2D(&depthTexDesc, NULL, &irradianceDepthTex);

	D3D11_DEPTH_STENCIL_VIEW_DESC  dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = NULL; //??? Acceptable?
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	ID3D11DepthStencilView* tempDSV;
	ID3D11DepthStencilView* irradianceDSV;
	hr = device->CreateDepthStencilView(depthTex, &dsvDesc, &tempDSV);
	hr = device->CreateDepthStencilView(irradianceDepthTex, &dsvDesc, &irradianceDSV);
	//_dynamicCubeMapDSV = new DepthStencilView(Device, depthTex, dsvDesc);
	tempStruct.dsv = tempDSV;
	irradianceStruct.dsv = irradianceDSV;

	//Util.ReleaseCom(ref depthTex);
	SafeReleaseD3D(depthTex);
	SafeReleaseD3D(irradianceDepthTex);

	// create the viewport for rendering the cubemap faces
	D3D11_VIEWPORT tempViewPort; // = new Viewport(0, 0, CubeMapSize, CubeMapSize, 0, 1.0f);
	ZeroMemory(&tempViewPort, sizeof(tempViewPort));
	tempViewPort.TopLeftX = 0.0f;
	tempViewPort.TopLeftY = 0.0f;
	tempViewPort.Width = (float)cubeMapSize;
	tempViewPort.Height = (float)cubeMapSize;
	tempViewPort.MinDepth = 0.0f;
	tempViewPort.MaxDepth = 1.0f;

	tempStruct.viewPort = tempViewPort;

	cubeMaps.push_back(tempStruct);
	entityData.cubeMapID = cubeMaps.size() - 1;

	// create the viewport for rendering the cubemap faces
	D3D11_VIEWPORT irradianceViewPort; // = new Viewport(0, 0, CubeMapSize, CubeMapSize, 0, 1.0f);
	ZeroMemory(&irradianceViewPort, sizeof(irradianceViewPort));
	irradianceViewPort.TopLeftX = 0.0f;
	irradianceViewPort.TopLeftY = 0.0f;
	irradianceViewPort.Width = (float)cubeMapSize;
	irradianceViewPort.Height = (float)cubeMapSize;
	irradianceViewPort.MinDepth = 0.0f;
	irradianceViewPort.MaxDepth = 1.0f;

	irradianceStruct.viewPort = irradianceViewPort;

	irradianceMaps.push_back(irradianceStruct);
}

ID3D11Device* D3D11BaseClass::GetDevice()
{
	return device;
}

ID3D11DeviceContext* D3D11BaseClass::GetDeviceContext()
{
	return deviceContext;
}

D3D11ShaderClass * D3D11BaseClass::GetShaderClass()
{
	return shaders;
}

D3D11BufferClass * D3D11BaseClass::GetBufferClass()
{
	return buffers;
}

D3D11MaterialClass * D3D11BaseClass::GetMaterialClass()
{
	return materials;
}

//ID3D11ShaderResourceView* D3D11::GetShadowMap()
//{
//	return shadowResourceView;
//}

void D3D11BaseClass::SetStandardSamplers()
{
	deviceContext->VSSetSamplers(0, 1, &wrapSample);
	deviceContext->PSSetSamplers(0, 1, &wrapSample);
}
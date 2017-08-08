#include "SGGD3D11Base.h"



void SGGD3D11Base::ThreadFunction()
{
	while (!stopThread)
	{

	}
}

HWND SGGD3D11Base::InitWindow(HINSTANCE hInstance, int width, int height)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &SGGD3D11Base::WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"SGG";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"SGG",
		L"SGG",
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

LRESULT SGGD3D11Base::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT SGGD3D11Base::CreateDirect3DContext(HWND wndHandle, int width, int height)
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

	//if (SUCCEEDED(hr))
	//{
	//	// get the address of the back buffer
	//	ID3D11Texture2D* pBackBuffer = nullptr;
	//	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//	// use the back buffer address to create the render target
	//	device->CreateRenderTargetView(pBackBuffer, NULL, &backbufferRTV);


	//	pBackBuffer->Release();

	//	D3D11_TEXTURE2D_DESC depthStencilDesc; // Create DepthStencil
	//	depthStencilDesc.Width = width;
	//	depthStencilDesc.Height = height;
	//	depthStencilDesc.MipLevels = 1;
	//	depthStencilDesc.ArraySize = 1;
	//	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	depthStencilDesc.SampleDesc.Count = 1;
	//	depthStencilDesc.SampleDesc.Quality = 0;
	//	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	//	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//	depthStencilDesc.CPUAccessFlags = 0;
	//	depthStencilDesc.MiscFlags = 0;

	//	device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer);
	//	device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView);

	//	// set the render target as the back buffer
	//	deviceContext->OMSetRenderTargets(1, &backbufferRTV, depthStencilView);
	//}
	return hr;
}

void SGGD3D11Base::SetViewport(int width, int height)
{
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
}

SGGD3D11Base::SGGD3D11Base(GraphicsSettings gSettings) : SGG3DAPI(gSettings)
{
	D3D11Specifics* temp = (D3D11Specifics*)(settings.apiSpecificsData);
	wndHandle = InitWindow(temp->hInstance, settings.windowWidth, settings.windowHeight);
	CreateDirect3DContext(wndHandle, settings.windowWidth, settings.windowHeight);
	SetViewport(settings.windowWidth, settings.windowHeight);

	constantBuffers = new SGGD3D11ConstantBuffers(device, deviceContext);
	rtAndDss = new SGGD3D11RenderTargetAndDepthStencil(device, deviceContext);
	srvs = new SGGD3D11ShaderResourceViews(device, deviceContext);
	shaders = new SGGD3D11Shaders(device, deviceContext);
	states = new SGGD3D11States(device, deviceContext);
	meshes = new SGGD3D11MeshBuffers(device, deviceContext);

	ShowWindow(wndHandle, SW_SHOWDEFAULT);
}

SGGD3D11Base::~SGGD3D11Base()
{
	//SafeReleaseD3D(backbufferRTV);
	SafeReleaseD3D(swapChain);
	SafeReleaseD3D(device);
	SafeReleaseD3D(deviceContext);

	//SafeReleaseD3D(depthStencilBuffer);
	//SafeReleaseD3D(depthStencilView);

	DestroyWindow(wndHandle);
}

void SGGD3D11Base::Stop()
{
	stopThread = true;
}

void SGGD3D11Base::CopySceneData(SGGScene * scene)
{
}

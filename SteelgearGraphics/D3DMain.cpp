#include "D3DMain.h"

D3DMain::D3DMain(HINSTANCE hInstance)
{
	wndHandle = InitWindow(hInstance);

	CreateDirect3DContext(wndHandle);

	SetViewport();

	//--------------------------------

	InitialiseStates();
	SetShadowResources();

	//textRenderer = TextClass::getInstance();
	//textRenderer->Setup(gDevice, gDeviceContext);
}


D3DMain::~D3DMain()
{

	SafeReleaseD3D(gBackbufferRTV);
	SafeReleaseD3D(gSwapChain);
	SafeReleaseD3D(gDevice);
	SafeReleaseD3D(gDeviceContext);

	SafeReleaseD3D(gDepthStencilBuffer);
	SafeReleaseD3D(gDepthStencilView);

	SafeReleaseD3D(sDepthStencilBuffer);
	SafeReleaseD3D(sDepthStencilView);
	SafeReleaseD3D(shadowRTV);
	SafeReleaseD3D(shadowResourceView);
	SafeReleaseD3D(shadowRaState);
	SafeReleaseD3D(shadowStencilState);

	DestroyWindow(wndHandle);

	SafeReleaseD3D(wrapSample);
	SafeReleaseD3D(sampleShadow);
	SafeReleaseD3D(transparency);
	SafeReleaseD3D(onState);
	SafeReleaseD3D(offState);

	//delete textRenderer;
}

HWND D3DMain::InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &D3DMain::WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_D3D_DEMO",
		L"JochEngine",
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

LRESULT CALLBACK D3DMain::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT D3DMain::CreateDirect3DContext(HWND wndHandle)
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
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);


		pBackBuffer->Release();

		D3D11_TEXTURE2D_DESC depthStencilDesc; // Create DepthStencil
		depthStencilDesc.Width = WIDTH;
		depthStencilDesc.Height = HEIGHT;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		gDevice->CreateTexture2D(&depthStencilDesc, 0, &gDepthStencilBuffer);
		gDevice->CreateDepthStencilView(gDepthStencilBuffer, 0, &gDepthStencilView);

		// set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencilView);
	}
	return hr;
}

void D3DMain::SetViewport()
{
	//D3D11_VIEWPORT vp;
	vp.Width = (float)WIDTH;
	vp.Height = (float)HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	svp.Width = SHADOWRES;
	svp.Height = SHADOWRES;
	svp.MinDepth = 0.0f;
	svp.MaxDepth = 1.0f;
	svp.TopLeftX = 0;
	svp.TopLeftY = 0;
}

HWND D3DMain::GetWindow()
{
	return wndHandle;
}

void D3DMain::SwapBackBuffer()
{
	gSwapChain->Present(0, 0);
}

void D3DMain::InitialiseStates()
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

	gDevice->CreateSamplerState(&samplerDesc, &wrapSample);

	gDeviceContext->VSSetSamplers(0, 1, &wrapSample);
	gDeviceContext->PSSetSamplers(0, 1, &wrapSample);

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

	gDevice->CreateSamplerState(&samplerDesc2, &sampleShadow);

	gDeviceContext->PSSetSamplers(1, 1, &sampleShadow);

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

	gDevice->CreateBlendState(&transparentDesc, &transparency);

	gDeviceContext->OMSetBlendState(transparency, NULL, 0xffffffff);

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

	gDevice->CreateRasterizerState(&onDesc, &onState);

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

	gDevice->CreateRasterizerState(&offDesc, &offState);

	D3D11_RASTERIZER_DESC sRasDesc;
	offDesc.FillMode = D3D11_FILL_SOLID;
	offDesc.CullMode = D3D11_CULL_NONE;
	offDesc.FrontCounterClockwise = false;
	offDesc.DepthBias = 0;
	offDesc.SlopeScaledDepthBias = 0.0f;
	offDesc.DepthBiasClamp = 0.0f;
	offDesc.DepthClipEnable = true;
	offDesc.ScissorEnable = false;
	offDesc.MultisampleEnable = false;
	offDesc.AntialiasedLineEnable = false;

	gDevice->CreateRasterizerState(&offDesc, &shadowRaState);

	//----------------------------------------------------------------------------------------------

	D3D11_DEPTH_STENCIL_DESC depthStateDesc;
	depthStateDesc.DepthEnable = true;
	depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStateDesc.StencilEnable = false;
	depthStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	gDevice->CreateDepthStencilState(&depthStateDesc, &shadowStencilState);

}

void D3DMain::SetShadowResources()
{
	HRESULT shadow;

	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = SHADOWRES;
	descDepth.Height = SHADOWRES;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS; //DXGI_FORMAT_R32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;


	ID3D11Texture2D *pDepthBuffer = NULL;
	shadow = gDevice->CreateTexture2D(&descDepth, NULL, &pDepthBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	shadow = gDevice->CreateDepthStencilView(pDepthBuffer, &descDSV, &sDepthStencilView);

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	shadow = gDevice->CreateShaderResourceView(pDepthBuffer, &srDesc, &shadowResourceView);

	pDepthBuffer->Release();

}

void D3DMain::setConstantBuffers(ID3D11Buffer** camera)
{
	gDeviceContext->VSSetConstantBuffers(0, 1, camera);
	gDeviceContext->GSSetConstantBuffers(0, 1, camera);
}

void D3DMain::SetBackFaceCull(bool on)
{
	if (on == true)
		gDeviceContext->RSSetState(onState);
	else
		gDeviceContext->RSSetState(offState);

}

void D3DMain::RenderShadows()
{
	gDeviceContext->OMSetBlendState(nullBlend, NULL, 0xffffffff);
	gDeviceContext->RSSetState(shadowRaState);
	gDeviceContext->OMSetDepthStencilState(shadowStencilState, 0);
	
	gDeviceContext->PSSetShaderResources(13, 1, &nullResourceView);
	gDeviceContext->OMSetRenderTargets(1, &nullRTV, sDepthStencilView);

	gDeviceContext->RSSetViewports(1, &svp);
	gDeviceContext->ClearDepthStencilView(sDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);

}

void D3DMain::Render(ID3D11Buffer** camera)
{
	gDeviceContext->OMSetBlendState(transparency, NULL, 0xffffffff);
	gDeviceContext->RSSetState(nullptr);
	gDeviceContext->OMSetDepthStencilState(nullptr, 0);

	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencilView);

	float clearColor[] = { 0, 0, 1, 1 };
	gDeviceContext->RSSetViewports(1, &vp);
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
	gDeviceContext->PSSetShaderResources(13, 1, &shadowResourceView);

	setConstantBuffers(camera);
		
}

ID3D11Device* D3DMain::GetDevice()
{
	return gDevice;
}

ID3D11DeviceContext* D3DMain::GetDeviceContext()
{
	return gDeviceContext;
}

ID3D11ShaderResourceView* D3DMain::GetShadowMap()
{
	return shadowResourceView;
}

void D3DMain::SetStandardSamplers()
{
	gDeviceContext->VSSetSamplers(0, 1, &wrapSample);
	gDeviceContext->PSSetSamplers(0, 1, &wrapSample);
}
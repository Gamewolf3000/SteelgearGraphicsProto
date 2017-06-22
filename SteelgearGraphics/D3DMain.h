#ifndef D3DMAIN_H
#define D3DMAIN_H
#pragma once

#include "ClassIncludes.h"
#include "TextClass.h"

class D3DMain
{
private:

	IDXGISwapChain* gSwapChain = nullptr;
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
	ID3D11RenderTargetView* gBackbufferRTV = nullptr;

	ID3D11Texture2D* gDepthStencilBuffer = NULL;
	ID3D11DepthStencilView* gDepthStencilView = NULL;

	ID3D11Texture2D* sDepthStencilBuffer = NULL;
	ID3D11DepthStencilView* sDepthStencilView = NULL;
	ID3D11RenderTargetView* shadowRTV = nullptr;
	ID3D11ShaderResourceView* shadowResourceView = nullptr;


	ID3D11RenderTargetView* nullRTV = nullptr;
	ID3D11ShaderResourceView* nullResourceView = nullptr;

	D3D11_VIEWPORT vp;
	D3D11_VIEWPORT svp;

	ID3D11SamplerState* wrapSample;
	ID3D11SamplerState* sampleShadow;
	ID3D11BlendState* transparency;
	ID3D11RasterizerState* onState;
	ID3D11RasterizerState* offState;
	ID3D11RasterizerState* shadowRaState;
	ID3D11DepthStencilState* shadowStencilState;

	ID3D11BlendState* nullBlend = nullptr;
	

	HWND wndHandle;

	HWND InitWindow(HINSTANCE hInstance);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	HRESULT CreateDirect3DContext(HWND wndHandle);
	void SetViewport();

	void InitialiseStates();
	void SetShadowResources();
	void setConstantBuffers(ID3D11Buffer** camera);

	//TextClass* textRenderer;

public:

	D3DMain(HINSTANCE hInstance);
	virtual ~D3DMain();

	HWND GetWindow();
	void SwapBackBuffer();
	void RenderShadows();
	void Render(ID3D11Buffer** camera);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11ShaderResourceView* GetShadowMap();

	void SetBackFaceCull(bool on);
	void SetStandardSamplers();

	
};

#endif

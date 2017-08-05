#pragma once

#include "SGG3DAPI.h"
#include "SGGEngineIncludes.h"
#include "SGGEntity.h"

#include "SGGD3D11Includes.h"

#include "SGGD3D11ConstantBuffers.h"
#include "SGGD3D11RenderTargetAndDepthStencil.h"
#include "SGGD3D11ShaderResourceViews.h"

class SGGD3D11Base : public SGG3DAPI
{
private:

	SGGD3D11ConstantBuffers* constantBuffers;
	SGGD3D11RenderTargetAndDepthStencil* rtAndDss;
	SGGD3D11ShaderResourceViews* srvs;

	void ThreadFunction();
	bool stopThread = false;

	HWND InitWindow(HINSTANCE hInstance, int width, int height);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	HRESULT CreateDirect3DContext(HWND wndHandle, int width, int height);
	void SetViewport(int width, int height);
	HWND wndHandle;

	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	//ID3D11RenderTargetView* backbufferRTV = nullptr;
	D3D11_VIEWPORT vp;
	//ID3D11Texture2D* depthStencilBuffer = NULL;
	//ID3D11DepthStencilView* depthStencilView = NULL;

public:
	SGGD3D11Base(GraphicsSettings gSettings);
	~SGGD3D11Base();

	void Stop();

	void HandleGraphicJobs(std::vector<GraphicJob*> jobs);
	void CopySceneData(SGGScene* scene);
};


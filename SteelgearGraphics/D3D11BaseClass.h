#ifndef D3D11_HANDLER_H
#define D3D11_HANDLER_H
#pragma once

#include "D3D11Includes.h"
#include "D3D11ShaderClass.h"
#include "D3D11BufferClass.h"
#include "D3D11MaterialClass.h"

#include "SGGEngineIncludes.h"

struct SGGEntity;
struct EntityMaterials;
struct CameraData;

struct LightStruct
{
	XMFLOAT4 positions[256];
	XMFLOAT4 colours[256];
	XMFLOAT4 lightRangesTypes[256];
};

struct ViewProjStruct
{
	XMFLOAT4X4 viewM;
	XMFLOAT4X4 projectionM;
};

struct CameraDataStruct
{
	XMFLOAT4 position;
	XMFLOAT4 lookAt;
};

struct CubeMapData
{
	ID3D11RenderTargetView** rtvs;
	ID3D11ShaderResourceView* srv;
	ID3D11DepthStencilView* dsv;
	D3D11_VIEWPORT viewPort;
};

class D3D11BaseClass
{
private:

	D3D11ShaderClass* shaders;
	D3D11BufferClass* buffers;
	D3D11MaterialClass* materials;

	ID3D11Buffer* bufferWorld = nullptr;
	ID3D11Buffer* bufferViewProj = nullptr;
	ID3D11Buffer* bufferCameraData = nullptr;
	ID3D11Buffer* bufferLight = nullptr;
	ID3D11Buffer* bufferCameraPosition = nullptr; // for shadow maps if I remember correctly?

	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ID3D11RenderTargetView* backbufferRTV = nullptr;

	ID3D11Texture2D* depthStencilBuffer = NULL;
	ID3D11DepthStencilView* depthStencilView = NULL;

	//ID3D11Texture2D* sDepthStencilBuffer = NULL;
	//ID3D11DepthStencilView* sDepthStencilView = NULL;
	//ID3D11RenderTargetView* shadowRTV = nullptr;
	//ID3D11ShaderResourceView* shadowResourceView = nullptr;


	ID3D11RenderTargetView* nullRTV = nullptr;
	ID3D11ShaderResourceView* nullResourceView = nullptr;

	D3D11_VIEWPORT vp;
	D3D11_VIEWPORT svp;

	ID3D11SamplerState* wrapSample = nullptr;
	ID3D11SamplerState* sampleShadow = nullptr;
	ID3D11BlendState* transparency = nullptr;
	ID3D11RasterizerState* onState = nullptr;
	ID3D11RasterizerState* offState = nullptr;
	//ID3D11RasterizerState* shadowRaState = nullptr;
	//ID3D11DepthStencilState* shadowStencilState = nullptr;

	ID3D11BlendState* nullBlend = nullptr;

	int cubeMapSize = 64; // if changed from 512 then it needs to be changed in the irradiance map shader as well!!!
	std::vector<CubeMapData> cubeMaps;
	std::vector<CubeMapData> irradianceMaps;

	HWND wndHandle;

	HWND InitWindow(HINSTANCE hInstance, int width, int height);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	HRESULT CreateDirect3DContext(HWND wndHandle, int width, int height);
	void SetViewport(int width, int height);
	void InitiateConstBuffers();

	void InitialiseStates();
	void SetShadowResources();

public:

	D3D11BaseClass(HINSTANCE hInstance, int width, int height, wchar_t directory[200]);
	virtual ~D3D11BaseClass();

	HWND GetWindow();
	void SwapBackBuffer();

	//void RenderShadows();

	void PreRender();
	void PreCubeMapRender(unsigned int cubeMapID, unsigned int rtvNR, bool isIrradianceMap);
	void SetMeshBuffers(int meshID);
	void SetShaders(int shaderID);
	void SetTransformBuffer(Float4x4 transform);
	void SetCameraBuffers(CameraData data);
	void SetViewProj(CameraData data);
	void SetMaterials(EntityMaterials data);
	void SetSingleMaterial(unsigned int materialID, MaterialType typeOfMaterial);
	void SetCubeMap(int cubeMapID);
	void SetTopology(D3D_PRIMITIVE_TOPOLOGY topology);
	void SetLightBuffers(std::vector<LightData> &lights, std::vector<Float3D> &positions);
	void Render(int nrOfIndices);

	void BuildCubeMapViews(EnvironmentMapData& entityData);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	D3D11ShaderClass* GetShaderClass();
	D3D11BufferClass* GetBufferClass();
	D3D11MaterialClass* GetMaterialClass();
	//ID3D11ShaderResourceView* GetShadowMap();

	void SetBackFaceCull(bool on);
	void SetStandardSamplers();


};

#endif

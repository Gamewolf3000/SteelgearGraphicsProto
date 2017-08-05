#pragma once

#include <vector>

#include "SGGEngineIncludes.h"

#include "SGGD3D11Includes.h"

struct RTData
{
	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
};

struct DSData
{
	ID3D11Texture2D* texture;
	ID3D11DepthStencilView* dsv;
	ID3D11ShaderResourceView* srv;
};

class SGGD3D11RenderTargetAndDepthStencil
{
private:

	std::vector<RTData> rtvs;
	std::vector<int> freeSpotsrt;
	std::vector<DSData> dss;
	std::vector<int> freeSpotsds;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

public:

	SGGD3D11RenderTargetAndDepthStencil(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~SGGD3D11RenderTargetAndDepthStencil();

	unsigned int CreateRenderTarget(unsigned int width, unsigned int height, bool usableAsShaderResource);
	void RemoveRenderTarget(unsigned int idOfRenderTargetToRemove);
	unsigned int CreateDepthStencil(unsigned int width, unsigned int height, bool usableAsShaderResource);
	void RemoveDepthStencil(unsigned int idOfDepthStencilToRemove);

	void ClearRenderTarget(unsigned int idOfRenderTargetToClear, const Float4D &clearColour);
	void ClearDepthStencil(unsigned int idOfDepthStencilToClear, float clearColour);

	void SetRTDS(unsigned int rtStartId, unsigned short int nrOfRT, unsigned int dsId);
	void SetRTShaderResourceViewPS(unsigned short int slotToSet, unsigned int srvToSet);
};


#pragma once

#include <vector>

#include "SGGEngineIncludes.h"

#include "SGGD3D11Includes.h"

class SGGD3D11States
{
private:

	std::vector<ID3D11BlendState*> blend;
	std::vector<int> freeSpotsbs;
	std::vector<ID3D11RasterizerState*> rasterizer;
	std::vector<int> freeSpotsrs;
	std::vector<ID3D11DepthStencilState*> depth;
	std::vector<int> freeSpotsdss;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

public:

	SGGD3D11States(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~SGGD3D11States();

	int CreateBlendState();
	void RemoveBlendState(int idToRemove);
	int CreateRasterizerState();
	void RemoveRasterizerState(int idToRemove);
	int CreateDepthStencilState();
	void RemoveDepthStencilState(int idToRemove);
};


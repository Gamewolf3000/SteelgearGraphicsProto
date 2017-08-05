#pragma once

#include <vector>

#include "SGGEngineIncludes.h"

#include "SGGD3D11Includes.h"

#include "DirectXHelpers.h"
#include "pch.h"
#include "PlatformHelpers.h"
#include "WICTextureLoader.h"

class SGGD3D11ShaderResourceViews
{
private:

	std::vector<ID3D11ShaderResourceView*> srvs;
	std::vector<int> freeSpots;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

public:

	SGGD3D11ShaderResourceViews(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~SGGD3D11ShaderResourceViews();

	unsigned int LoadTexture(std::string file);
	void RemoveTexture(unsigned int idOfBufferToRemove);

	void SetShaderResourceViewPS(unsigned short int slotToSet, unsigned int srvToSet);
};
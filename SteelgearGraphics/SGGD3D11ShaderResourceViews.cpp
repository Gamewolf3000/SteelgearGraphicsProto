#include "SGGD3D11ShaderResourceViews.h"

SGGD3D11ShaderResourceViews::SGGD3D11ShaderResourceViews(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
}

SGGD3D11ShaderResourceViews::~SGGD3D11ShaderResourceViews()
{
	for (int i = 0; i < srvs.size(); i++)
	{
		SafeReleaseD3D(srvs[i]);
	}
}

unsigned int SGGD3D11ShaderResourceViews::LoadTexture(std::string file)
{
	HRESULT hr;
	unsigned int textureSlot = -1;

	if (freeSpots.size() != 0)
	{
		textureSlot = freeSpots[freeSpots.size() - 1];
		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
	}
	else
	{
		textureSlot = srvs.size();
		ID3D11ShaderResourceView* temp;
		srvs.push_back(temp);
	}

	std::wstring temp(file.begin(), file.end());
	hr = CreateWICTextureFromFile(device, deviceContext, temp.c_str() , 0, &srvs[textureSlot], NULL);

	return textureSlot;
}

void SGGD3D11ShaderResourceViews::RemoveTexture(unsigned int idOfBufferToRemove)
{
	SafeReleaseD3D(srvs[idOfBufferToRemove]);
	freeSpots.push_back(idOfBufferToRemove);
}

void SGGD3D11ShaderResourceViews::SetShaderResourceViewPS(unsigned short int slotToSet, unsigned int srvToSet)
{
	deviceContext->PSSetShaderResources(slotToSet, 1, &srvs[srvToSet]);
}

#include "SGGD3D11ConstantBuffers.h"

SGGD3D11ConstantBuffers::SGGD3D11ConstantBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
}

SGGD3D11ConstantBuffers::~SGGD3D11ConstantBuffers()
{
	for (int i = 0; i < cBuffers.size(); i++)
	{
		SafeReleaseD3D(cBuffers[i]);
	}
}

unsigned int SGGD3D11ConstantBuffers::MakeBuffer(void * data, unsigned int size)
{
	int bufferSlot = -1;


	if (freeSpots.size() != 0)
	{
		bufferSlot = freeSpots[freeSpots.size() - 1];
		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
	}
	else
	{
		bufferSlot = cBuffers.size();
		ID3D11Buffer* temp;
		cBuffers.push_back(temp);
	}

	D3D11_BUFFER_DESC constDesc;
	memset(&constDesc, 0, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.Usage = D3D11_USAGE_DYNAMIC;
	constDesc.ByteWidth = size;
	constDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	device->CreateBuffer(&constDesc, &initData, &cBuffers[bufferSlot]);

	return bufferSlot;
}

void SGGD3D11ConstantBuffers::UpdateBuffer(unsigned int idOfBufferToUpdate, void * data, unsigned int size)
{
	D3D11_MAPPED_SUBRESOURCE resource;

	deviceContext->Map(cBuffers[idOfBufferToUpdate], 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	deviceContext->Unmap(cBuffers[idOfBufferToUpdate], 0);
}

void SGGD3D11ConstantBuffers::RemoveBuffer(unsigned int idOfBufferToRemove)
{
	SafeReleaseD3D(cBuffers[idOfBufferToRemove]);
	freeSpots.push_back(idOfBufferToRemove);
}

void SGGD3D11ConstantBuffers::SetBufferVS(unsigned short int slotToSet, unsigned int bufferToSet)
{
	deviceContext->VSSetConstantBuffers(slotToSet, 1, &cBuffers[bufferToSet]);
}

void SGGD3D11ConstantBuffers::SetBufferGS(unsigned short int slotToSet, unsigned int bufferToSet)
{
	deviceContext->GSSetConstantBuffers(slotToSet, 1, &cBuffers[bufferToSet]);
}

void SGGD3D11ConstantBuffers::SetBufferPS(unsigned short int slotToSet, unsigned int bufferToSet)
{
	deviceContext->PSSetConstantBuffers(slotToSet, 1, &cBuffers[bufferToSet]);
}

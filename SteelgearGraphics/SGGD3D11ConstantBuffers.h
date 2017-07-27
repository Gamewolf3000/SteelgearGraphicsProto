#pragma once

#include <vector>

#include "SGGEngineIncludes.h"

#include "SGGD3D11Includes.h"

class SGGD3D11ConstantBuffers
{
private:

	std::vector<ID3D11Buffer*> cBuffers;
	std::vector<int> freeSpots;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

public:

	SGGD3D11ConstantBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~SGGD3D11ConstantBuffers();

	unsigned int MakeBuffer(void* data, unsigned int size);
	void UpdateBuffer(unsigned int idOfBufferToUpdate, void* data, unsigned int size);
	void RemoveBuffer(unsigned int idOfBufferToRemove);

	void SetBufferVS(unsigned short int slotToSet, unsigned int bufferToSet);
	void SetBufferGS(unsigned short int slotToSet, unsigned int bufferToSet);
	void SetBufferPS(unsigned short int slotToSet, unsigned int bufferToSet);
};
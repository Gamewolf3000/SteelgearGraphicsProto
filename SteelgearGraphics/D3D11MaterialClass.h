#ifndef D3D_MATERIAL_CLASS_H
#define D3D_MATERIAL_CLASS_H
#pragma once

#include "EngineIncludes.h"
#include "D3D11Includes.h"

#include "DirectXHelpers.h"
#include "pch.h"
#include "PlatformHelpers.h"
#include "WICTextureLoader.h"

struct LoadedMaterial
{
	std::string fileName = "";
	ID3D11ShaderResourceView* resource = nullptr;
	MaterialType type;
};

class D3D11MaterialClass
{
private:
	std::vector<LoadedMaterial> loadedMaterials;
	std::vector<int> freeSpots;
	std::string* materialDirectory;


public:

	D3D11MaterialClass(wchar_t directory[200]);
	virtual ~D3D11MaterialClass();
	
	void LoadMaterial(MaterialData& data, ID3D11Device* device, ID3D11DeviceContext* devicecontext);
	void RemoveMaterial(std::string fileName);
	ID3D11ShaderResourceView** GetMaterial(unsigned int materialID);


};

#endif

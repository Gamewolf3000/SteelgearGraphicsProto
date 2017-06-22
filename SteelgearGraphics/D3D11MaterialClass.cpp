#include "D3D11MaterialClass.h"

D3D11MaterialClass::D3D11MaterialClass(wchar_t directory[200])
{
	std::wstring temp(directory);
	materialDirectory = new std::string(temp.begin(), temp.end());
}

D3D11MaterialClass::~D3D11MaterialClass()
{
	delete materialDirectory;

	for (int i = 0; i < loadedMaterials.size(); i++)
	{
		SafeReleaseD3D(loadedMaterials[i].resource);
	}
}

void D3D11MaterialClass::LoadMaterial(MaterialData & data, ID3D11Device * device, ID3D11DeviceContext * devicecontext)
{
	int vectorSpot;
	HRESULT hr;

	for (unsigned int i = 0; i < loadedMaterials.size(); i++)
	{
		if (data.fileName == loadedMaterials[i].fileName) // Look for already loaded material
		{
			return;
		}
	}


	wchar_t materialPath[250] = L"";
	std::wstring tempWDir(materialDirectory->begin(), materialDirectory->end());
	std::wstring tempWName(data.fileName.begin(), data.fileName.end());

	wcscat_s(materialPath, tempWDir.c_str());
	wcscat_s(materialPath, L"\\Materials\\");

	if (freeSpots.size() > 0)
	{
		vectorSpot = freeSpots[freeSpots.size() - 1];
		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
		loadedMaterials[vectorSpot].fileName = data.fileName;

		if (data.type == diffuse)
		{
			loadedMaterials[vectorSpot].type = diffuse;
			wcscat_s(materialPath, L"DiffuseTextures\\");
		}
		else if (data.type == roughness)
		{
			loadedMaterials[vectorSpot].type = roughness;
			wcscat_s(materialPath, L"RoughnessTextures\\");
		}
		else if (data.type == metallic)
		{
			loadedMaterials[vectorSpot].type = metallic;
			wcscat_s(materialPath, L"MetallicTextures\\");
		}
		else if (data.type == normal)
		{
			loadedMaterials[vectorSpot].type = normal;
			wcscat_s(materialPath, L"NormalMaps\\");
		}
		else if (data.type == displacement)
		{
			loadedMaterials[vectorSpot].type = displacement;
			wcscat_s(materialPath, L"DisplacementMaps\\");
		}

		wcscat_s(materialPath, tempWName.c_str());

		hr = CreateWICTextureFromFile(device, devicecontext, materialPath, 0, &loadedMaterials[vectorSpot].resource, NULL);

		if (hr != S_OK)
		{
			ErrorCheck(6000, L"Error loading texture " + tempWName);
		}

	}
	else
	{
		LoadedMaterial temp;
		temp.fileName = data.fileName;
		vectorSpot = loadedMaterials.size();

		if (data.type == diffuse)
		{
			temp.type = diffuse;
			wcscat_s(materialPath, L"DiffuseTextures\\");
		}
		else if (data.type == roughness)
		{
			temp.type = roughness;
			wcscat_s(materialPath, L"RoughnessTextures\\");
		}
		else if (data.type == metallic)
		{
			temp.type = metallic;
			wcscat_s(materialPath, L"MetallicTextures\\");
		}
		else if (data.type == normal)
		{
			temp.type = normal;
			wcscat_s(materialPath, L"NormalMaps\\");
		}
		else if (data.type == displacement)
		{
			temp.type = displacement;
			wcscat_s(materialPath, L"DisplacementMaps\\");
		}

		wcscat_s(materialPath, tempWName.c_str());

		hr = CreateWICTextureFromFile(device, devicecontext, materialPath, 0, &temp.resource, NULL);

		if (hr != S_OK)
		{
			ErrorCheck(6000, L"Error loading texture " + tempWName);
		}

		loadedMaterials.push_back(temp);
	}
}

void D3D11MaterialClass::RemoveMaterial(std::string fileName)
{
	int materialID = -1;

	for (unsigned int i = 0; i < loadedMaterials.size(); i++)
	{
		if (fileName == loadedMaterials[i].fileName)
		{
			materialID = i;
			break;
		}
	}

	SafeReleaseD3D(loadedMaterials[materialID].resource);
	loadedMaterials[materialID].resource = nullptr;
	loadedMaterials[materialID].fileName = "";

	freeSpots.push_back(materialID);
}

ID3D11ShaderResourceView ** D3D11MaterialClass::GetMaterial(unsigned int materialID)
{
	return &loadedMaterials[materialID].resource;
}
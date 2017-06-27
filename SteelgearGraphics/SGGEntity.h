#pragma once

#include <string>

#include "SGGEngineIncludes.h"
#include "TransformHandler.h"
#include "MeshHandler.h"
#include "ShaderHandler.h"
#include "CameraHandler.h"
#include "MaterialHandler.h"
#include "LightHandler.h"

struct SGGEntityMaterials
{
	unsigned int diffuseTex = 0;
	unsigned int roughnessTex = 1;
	unsigned int metallicTex = 2;

	unsigned int normalMap = 3;
	unsigned int displacementMap = 4;

	unsigned long long int comparator = -1; // Max value

	bool operator==(SGGEntityMaterials const& other)
	{
		return this->comparator == other.comparator;
	}

	bool operator>(SGGEntityMaterials const& other)
	{
		return this->comparator > other.comparator;
	}

	bool operator<(SGGEntityMaterials const& other)
	{
		return this->comparator < other.comparator;
	}

	void SetMaterials(int dTex, int rTex, int mTex, int nMap, int dMap)
	{
		this->diffuseTex = dTex;
		this->roughnessTex = rTex;
		this->metallicTex = mTex;
		this->normalMap = nMap;
		this->displacementMap = dMap;
	}

	void UpdateComparator()
	{
		comparator = diffuseTex * pow(10, 0) + roughnessTex * pow(10, 3) + metallicTex * pow(10, 6) + normalMap * pow(10, 9) + displacementMap * pow(10, 12); // Assuming that there are no more than 999 of each type of material
	}
};

class SGGEntity
{
private:

	friend class SGGEntityHandler;
	friend class SGGEngine;
	friend class EntitySorter;
	friend class TransformHandler;
	friend class MeshHandler;
	friend class ShaderHandler;
	friend class CameraHandler;
	friend class MaterialHandler;
	friend class LightHandler;

	std::string identifier; // to be able to find a specific entity for for example copying, or binding as child/parent
	int idNumber; // Number given by Entity handler

	TransformData transform;

	//Ids for knowing which components the entity use

	int meshID = -1;
	SGGEntityMaterials materialsIDs;
	int shaderID = -1;
	int cameraID = -1;
	int lightID = -1;

	bool needsSorting = true;


public:
	SGGEntity();
	~SGGEntity();

	bool operator==(SGGEntity const& other)
	{
		return (this->idNumber == other.idNumber);
	}
};


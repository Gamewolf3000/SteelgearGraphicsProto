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

	bool operator==(SGGEntityMaterials const& other)
	{
		if (this->diffuseTex == other.diffuseTex && this->roughnessTex == other.roughnessTex && this->metallicTex == other.metallicTex && this->normalMap == other.normalMap && this->displacementMap == other.displacementMap)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void SetMaterials(int dTex, int rTex, int mTex, int nMap, int dMap)
	{
		this->diffuseTex = dTex;
		this->roughnessTex = rTex;
		this->metallicTex = mTex;
		this->normalMap = nMap;
		this->displacementMap = dMap;
	}
};

class SGGEntity
{
private:

	friend class EntityHandler;
	friend class JochEngine;
	friend class TransformHandler;
	friend class MeshHandler;
	friend class ShaderHandler;
	friend class CameraHandler;
	friend class MaterialHandler;
	friend class LightHandler;

	std::string identifier; // to be able to find a specific entity for for example copying, or binding as child/parent
	int idNumber; // Number given by Entity handler

	Entity* parent = nullptr; // To be able to tell parent that child is removed
	std::vector<Entity*> children; // Update children with position and direction when parent changes


	//Ids for knowing which components the entity use

	int meshID = -1;
	SGGEntityMaterials materialsIDs;
	int shaderID = -1;
	int transformID = -1;
	int cameraID = -1;
	int lightID = -1;



public:
	SGGEntity();
	~SGGEntity();

	bool operator==(SGGEntity const& other)
	{
		if (this->identifier == other.identifier && this->idNumber == other.idNumber && this->parent == other.parent && this->children == other.children && this->meshID == other.meshID && this->materialsIDs == other.materialsIDs && this->shaderID == other.shaderID && this->transformID == other.transformID && this->cameraID == other.cameraID && this->lightID == other.lightID)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};


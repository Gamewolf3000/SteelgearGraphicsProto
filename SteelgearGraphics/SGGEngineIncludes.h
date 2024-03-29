#pragma once

#include "SGGMath.h"

#ifdef _WIN32

#include <Windows.h>

#endif // _WIN32

#define TRANSFORMSLOT 0

#define NROFCONSTBUFFERSLOTS 16
#define NROFPIXELSHADERTEXTURESLOTS 16


enum GraphicsAPI
{
	D3D11
};

struct APISpecifics
{

};

struct D3D11Specifics : public APISpecifics
{
	HINSTANCE hInstance;
};

struct GraphicsSettings
{

	GraphicsAPI gAPI = GraphicsAPI::D3D11;
	APISpecifics* apiSpecificsData = nullptr;
	unsigned int windowWidth = 1280;
	unsigned int windowHeight = 720;

};

enum SortType
{
	PIPELINE,
	MESH,
	MATERIALS,
	NONE

};

struct EntitySortingSettings
{
	SortType order[3] = { PIPELINE, MESH, MATERIALS };
	unsigned int incrementAmount = 10;
};

struct SGGSettings
{
	GraphicsSettings graphics;
	EntitySortingSettings sorting;
};

class SGGEntity;

struct SortedEntitiesData
{
	SGGEntity* arr;
	unsigned int nrOfEntities;
};

// 1 = Succeded no problems, 0 = Something might be wrong but not critical, -1 = Critical error the program should terminate
struct FunctionResult
{
	int successLevel;
	unsigned int errorID;
	std::wstring errorMessage;
};

struct StandardVertex
{
	Float3D position = Float3D(0.0f, 0.0f, 0.0f);
	Float2D uv = Float2D(0.0f, 0.0f);
	Float3D normal = Float3D(0.0f, 0.0f, 0.0f);

	Float3D tangent = Float3D(0.0f, 0.0f, 0.0f);
	Float3D bitangent = Float3D(0.0f, 0.0f, 0.0f);
};

struct MeshData
{
	StandardVertex* vertexBuffer = nullptr;
	unsigned int* indexBuffer = nullptr;
	unsigned int nrOfIndices = 0;
	bool render = true;

	unsigned int nrOfUsers = 0;
	std::string identifier = "";
};

enum MaterialType { diffuse, roughness, metallic, normal, displacement };

struct MaterialData
{
	std::string fileName = "";
	MaterialType type;
	unsigned int nrOfFrames = 1;
	unsigned int currentFrame = 0;

	unsigned int nrOfUsers = 0;
};

#define POINTLIGHT 0

struct LightData
{
	Float3D offset = Float3D(0, 0, 0);
	//Float3D rotation = Float3D(0, 0, 0);
	Float3D colour = Float3D(0, 0, 0);
	float range = -1;

	int typeOfLight = -1;
	bool active = true;
};

struct TransformData
{
	TransformData()
	{
		position = VecCreate(0.0f, 0.0f, 0.0f, 1.0f);
		scale = VecCreate(1.0f, 1.0f, 1.0f, 1.0f);
		rotation = MatrixIdentity;

		direction = VecCreate(0.0f, 0.0f, -1.0f, 0.0f);
		right = VecCreate(-1.0f, 0.0f, 0.0f, 0.0f);
		up = VecCreate(0.0f, 1.0f, 0.0f, 0.0f);
	}

	Vec position; // (0, 0, 0) as standard
	Vec scale; // (1, 1, 1) as standard
	Matrix rotation; // identity matrix as standard

	Vec direction; // (0, 0, -1) as standard
	Vec right; // (-1, 0, 0) as standard
	Vec up; // (0, 1, 0) as standard

	TransformData* parent = nullptr;
};

enum GraphicJobType
{
	MESH_JOB,
	TEXTURE_JOB,
	PIPELINE_JOB
};

struct GraphicJob
{
	GraphicJobType type;
};

//enum EnvironmentMap { noMap, skycubeMap_static, skycubeMap_dynamic, staticMap, dynamicMap };
//
//struct EnvironmentMapData
//{
//	EnvironmentMap typeOfMap = noMap;
//
//	int updateGroup = 0;
//	int updateDelay = 1;
//	int currentDelay = 0;
//
//	int cubeMapID = -1;
//
//	bool needsUpdate = false;
//
//	bool operator==(EnvironmentMapData const& other)
//	{
//		if (this->typeOfMap == other.typeOfMap && this->updateGroup == other.updateGroup && this->updateDelay == other.updateDelay && this->currentDelay == other.currentDelay && this->cubeMapID == other.cubeMapID && this->needsUpdate == other.needsUpdate)
//		{
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//};
//
//struct EnvironmentMapJob
//{
//	EnvironmentMap typeOfMap = noMap;
//	int entityID = -1;
//};
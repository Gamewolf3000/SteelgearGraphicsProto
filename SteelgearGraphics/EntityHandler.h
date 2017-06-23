//#ifndef ENTITY_HANDLER_H
//#define ENTITY_HANDLER_H
//#pragma once
//
//#include "EngineIncludes.h"
////BoundingHandler.h
//#include "TransformHandler.h"
//#include "MeshHandler.h"
//#include "ShaderHandler.h"
//#include "CameraHandler.h"
//#include "MaterialHandler.h"
//#include "LightHandler.h"
//
//// Create an Entity handler which keeps track and handles all entities
//
//struct EntityMaterials
//{
//	unsigned int diffuseTex = 0;
//	unsigned int roughnessTex = 1;
//	unsigned int metallicTex = 2;
//
//	unsigned int normalMap = 3;
//	unsigned int displacementMap = 4;
//
//	bool operator==(EntityMaterials const& other)
//	{
//		if (this->diffuseTex == other.diffuseTex && this->roughnessTex == other.roughnessTex && this->metallicTex == other.metallicTex && this->normalMap == other.normalMap && this->displacementMap == other.displacementMap)
//		{
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//	void SetMaterials(int dTex, int rTex, int mTex, int nMap, int dMap)
//	{
//		this->diffuseTex = dTex;
//		this->roughnessTex = rTex;
//		this->metallicTex = mTex;
//		this->normalMap = nMap;
//		this->displacementMap = dMap;
//	}
//};
//
//struct Entity
//{
//	friend class EntityHandler;
//	friend class JochEngine;
//	friend class TransformHandler;
//	friend class MeshHandler;
//	friend class ShaderHandler;
//	friend class CameraHandler;
//	friend class MaterialHandler;
//	friend class LightHandler;
//
//private:
//	std::string identifier; // to be able to find a specific entity for for example copying, or binding as child/parent
//	int idNumber; // Number given by Entity handler
//
//	//These should maybe be moved into the transformhandler?
//	Entity* parent = nullptr; // To be able to tell parent that child is removed
//	std::vector<Entity*> children; // Update children with position and direction when parent changes
//
//
//	//int ids for knowing which components the entity use
//
//	int meshID = -1;
//	EntityMaterials materialsIDs;
//	int shaderID = -1;
//	int transformID = -1;
//	int cameraID = -1;
//	int lightID = -1;
//
//	EnvironmentMapData environmentMapInfo;
//
//public:
//
//	bool operator==(Entity const& other)
//	{
//		if (this->identifier == other.identifier && this->idNumber == other.idNumber && this->parent == other.parent && this->children == other.children && this->meshID == other.meshID && this->materialsIDs == other.materialsIDs && this->shaderID == other.shaderID && this->transformID == other.transformID && this->cameraID == other.cameraID && this->lightID == other.lightID && this->environmentMapInfo == other.environmentMapInfo)
//		{
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//};
//
//
//class EntityHandler
//{
//	friend class JochEngine;
//
//private:
//	std::vector<Entity*> entities;
//	std::vector<int> freeSpots;
//
//	TransformHandler* transformHandler;
//	MeshHandler* meshHandler;
//	ShaderHandler* shaderHandler;
//	CameraHandler* cameraHandler;
//	MaterialHandler* materialHandler;
//	LightHandler* lightHandler;
//
//	std::vector<MeshData>& GetMeshJobs();
//	void ClearMeshJobs();
//	std::vector<ShaderData>& GetShaderJobs();
//	void ClearShaderJobs();
//	std::vector<MaterialData>& GetMaterialJobs();
//	void ClearMaterialJobs();
//	std::vector<EnvironmentMapJob>& GetEnvironmentMapJobs();
//	void ClearEnvironmentMapJobs();
//
//	std::vector<EnvironmentMapJob> environmentMapJobs;
//
//	Float4x4 GetEntityTransform(Entity& entity);
//	LightData GetLightData(Entity& entity);
//
//	std::vector<Entity*>& GetEntities();
//	CameraData GetActiveCamera();
//
//public:
//
//	EntityHandler(wchar_t directory[200]);
//	virtual ~EntityHandler();
//
//	Entity* CreateEntity();
//	Entity* CreateEntity(std::string identifier);
//	void DeleteEntity(Entity& entity);
//
//	TransformHandler* Transform();
//	MeshHandler* Mesh();
//	ShaderHandler* Shader();
//	CameraHandler* Camera();
//	MaterialHandler* Material();
//	LightHandler* Light();
//
//	void SetEnvironmentMap(Entity& entity, EnvironmentMap typeOfMap, int updateDelay = 1);
//	
//
//
//};
//
//#endif

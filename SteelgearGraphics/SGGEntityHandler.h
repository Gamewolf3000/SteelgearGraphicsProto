#pragma once

#include <vector>

#include "SGGEntity.h"

class SGGEntityHandler
{
	friend class SGGEngine;

private:
	std::vector<SGGEntity*> entities;
	std::vector<int> freeSpots;

	MeshHandler* meshHandler;
	ShaderHandler* pipelineHandler;
	LightHandler* lightHandler;
	CameraHandler* cameraHandler;
	MaterialHandler* materialHandler;
	TransformHandler* transformHandler;

	//const std::vector<SGGMeshJob>& GetMeshJobs();
	//void ClearMeshJobs();
	//const std::vector<SGGPipelineJob>& GetShaderJobs();
	//void ClearShaderJobs();
	//const std::vector<SGGMaterialJob>& GetMaterialJobs();
	//void ClearMaterialJobs();
	//const std::vector<SGGLightJob>& GetLightJobs();
	//void ClearLightJobs();
	//const std::vector<SGGCameraJob>& GetCameraJobs();
	//void ClearCameraJobs();

	//std::vector<EnvironmentMapJob>& GetEnvironmentMapJobs();
	//void ClearEnvironmentMapJobs();
	//std::vector<EnvironmentMapJob> environmentMapJobs;

	const std::vector<SGGEntity*>& GetEntities();

	void InsertEntity(SGGEntity* entity);

public:

	SGGEntityHandler(MeshHandler* mesh, ShaderHandler* pipeline, LightHandler* light, CameraHandler* camera, MaterialHandler* material,	TransformHandler* transform);
	virtual ~SGGEntityHandler();

	SGGEntity* CreateEntity();
	SGGEntity* CreateEntity(std::string identifier);
	void DeleteEntity(SGGEntity& entity);

	TransformHandler* Transform();
	MeshHandler* Mesh();
	ShaderHandler* Shader();
	CameraHandler* Camera();
	MaterialHandler* Material();
	LightHandler* Light();

	//void SetEnvironmentMap(Entity& entity, EnvironmentMap typeOfMap, int updateDelay = 1);
};

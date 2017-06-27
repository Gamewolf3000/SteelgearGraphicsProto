#pragma once

#ifdef _WIN32

#include <Windows.h>

#endif // _WIN32



#include "TimerClass.h"
//#include "D3D11BaseClass.h"

#include "EntityHandler.h"
#include "CameraHandler.h"
#include "LightHandler.h"
#include "MaterialHandler.h"
#include "MeshHandler.h"
#include "ShaderHandler.h"
#include "TransformHandler.h"

#include "SGGScene.h"
#include "SGG3DAPI.h"
#include "SGGEngineIncludes.h"

#include <iostream>

class SGGEngine
{
private:
	static bool instanceFlag;
	static SGGEngine *single;
	SGGEngine()
	{
		//private constructor
	}

	SGGSettings engineSettings;

	SGG3DAPI* graphicsApi;
	//D3D11BaseClass* d3d;
	//Controls* controller;
	//Audio* audio;
	//TimerClass* timer;

	MeshHandler* meshHandler;
	ShaderHandler* pipelineHandler;
	LightHandler* lightHandler;
	CameraHandler* cameraHandler;
	MaterialHandler* materialHandler;
	TransformHandler* transformHandler;

	wchar_t directory[200];

	//void GatherJobs(Scene* scene);
	//void GatherLights(Scene* scene, std::vector<Entity*> entities);

	//void HandleEnvironmentMapRendering(Scene* scene, std::vector<Entity*>& entities);
	//void HandleIrradianceMaps(Scene* scene, std::vector<Entity*>& entities, std::vector<int> irradianceMapToUpdate);
	//Entity* quadEnt;

	//std::vector<Entity> lastSeenEntities;
	//std::vector<RenderJob> renderJobs;
	//bool NeedToSort(std::vector<Entity*>& entities);
	//void SortRenderJobs(Scene* scene, std::vector<Entity*>& entities, std::vector<RenderJob>& returnVec);

public:
	static SGGEngine* getInstance();
	void Setup(HINSTANCE hInstance, SGGSettings settings);
	~SGGEngine();

	//TimerClass* GetTimer();
	//Audio* Sound();
	SGGScene* CreateScene();

	void RenderScene(SGGScene* scene);

	//void DisplayFPS();

	wchar_t* GetDirectory();
};


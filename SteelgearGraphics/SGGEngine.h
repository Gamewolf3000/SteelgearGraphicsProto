#pragma once

#include "TimerClass.h"

#include "EntityHandler.h"
#include "D3D11BaseClass.h"
#include "SGGScene.h"

class SGGEngine
{
private:
	static bool instanceFlag;
	static SGGEngine *single;
	SGGEngine()
	{
		//private constructor
	}

	SGG3DAPI graphicsApi;
	//D3D11BaseClass* d3d;
	//Controls* controller;
	//Audio* audio;
	//TimerClass* timer;

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
	void Setup(HINSTANCE hInstance);
	~SGGEngine();

	HWND GetWindow();
	TimerClass* GetTimer();

	//Audio* Sound();
	Scene* CreateScene();

	void RenderScene(Scene* scene);

	void DisplayFPS();

	wchar_t* GetDirectory();
};


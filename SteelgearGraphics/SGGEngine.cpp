#include "SGGEngine.h"

bool SGGEngine::instanceFlag = false;
SGGEngine* SGGEngine::single = NULL;

SGGEngine * SGGEngine::getInstance()
{
	if (!instanceFlag)
	{
		single = new SGGEngine;
		instanceFlag = true;
		return single;
	}
	else
	{
		return single;
	}
}

void SGGEngine::Setup(HINSTANCE hInstance, SGGSettings settings)
{
	GetCurrentDirectory(200, directory);

	engineSettings = settings;

	transformHandler = new TransformHandler;
	meshHandler = new MeshHandler(directory);
	pipelineHandler = new ShaderHandler(directory);
	lightHandler = new LightHandler;
	cameraHandler = new CameraHandler;
	materialHandler = new MaterialHandler;

	//if(settings.graphics.gAPI == GraphicsAPI::D3D11)
		//graphicsApi = new D3D11BaseClass(hInstance, directory, settings.graphics);
}

SGGEngine::~SGGEngine()
{
	delete graphicsApi;

	instanceFlag = false;
}

SGGScene * SGGEngine::CreateScene()
{
	return new SGGScene(engineSettings.sorting, meshHandler, pipelineHandler, lightHandler, cameraHandler, materialHandler, transformHandler);
}

void SGGEngine::RenderScene(SGGScene * scene)
{
	SortedEntitiesData temp = scene->Entity()->GetEntities();
}

wchar_t * SGGEngine::GetDirectory()
{
	return directory;
}

#include "SGGEngine.h"



SGGEngine::SGGEngine()
{
}


SGGEngine * SGGEngine::getInstance()
{
	if (!instanceFlag)
	{
		single = new SGGEngine();
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

	if(settings.graphics.gAPI == GraphicsAPI::D3D11)
		graphicsApi = new D3D11BaseClass(hInstance, directory, settings.graphics);
}

SGGEngine::~SGGEngine()
{
	delete graphicsApi;

	instanceFlag = false;
}

SGGScene * SGGEngine::CreateScene()
{
	return new SGGScene(directory);
}

void SGGEngine::RenderScene(SGGScene * scene)
{

}

wchar_t * SGGEngine::GetDirectory()
{
	return directory;
}

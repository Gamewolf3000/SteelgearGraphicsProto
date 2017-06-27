#include "SGGEngine.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	SGGSettings engineSettings;

	SGGEngine* engine = SGGEngine::getInstance();
	engine->Setup(hInstance, engineSettings);

	SGGScene* testScene = engine->CreateScene();

	SGGEntity* ent1 = testScene->Entity()->CreateEntity();
	SGGEntity* ent2 = testScene->Entity()->CreateEntity();
	SGGEntity* ent3 = testScene->Entity()->CreateEntity();

	testScene->Entity()->Mesh()->BindMesh(*ent3, "Sphere.obj");
	testScene->Entity()->Mesh()->BindMesh(*ent2, "The_Mighty_Cube2.obj");
	testScene->Entity()->Mesh()->BindMesh(*ent1, "Sphere.obj");

	testScene->Entity()->Shader()->BindShaders(*ent1, "testshader", true, false, false, false, true);
	testScene->Entity()->Shader()->BindShaders(*ent2, "testshader", true, false, false, false, true);
	testScene->Entity()->Shader()->BindShaders(*ent3, "testshader", true, false, false, false, true);

	testScene->Entity()->Material()->BindMaterial(*ent1, "corkboard3b-albedo.png", diffuse);
	testScene->Entity()->Material()->BindMaterial(*ent2, "oakfloor2_basecolor.png", diffuse);

	engine->RenderScene(testScene);

	getchar();
}
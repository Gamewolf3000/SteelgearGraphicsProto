#include "SGGScene.h"



SGGScene::SGGScene(MeshHandler* mesh, ShaderHandler* pipeline, LightHandler* light, CameraHandler* camera, MaterialHandler* material, TransformHandler* transform)
{
	entHandler = new SGGEntityHandler(mesh, pipeline, light, camera, material, transform);
}


SGGScene::~SGGScene()
{
}

SGGEntityHandler * SGGScene::Entity()
{
	return entHandler;
}

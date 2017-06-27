#include "SGGScene.h"



SGGScene::SGGScene(EntitySortingSettings& sortingSettings, MeshHandler* mesh, ShaderHandler* pipeline, LightHandler* light, CameraHandler* camera, MaterialHandler* material, TransformHandler* transform)
{
	entHandler = new SGGEntityHandler(sortingSettings, mesh, pipeline, light, camera, material, transform);
}


SGGScene::~SGGScene()
{
}

SGGEntityHandler * SGGScene::Entity()
{
	return entHandler;
}

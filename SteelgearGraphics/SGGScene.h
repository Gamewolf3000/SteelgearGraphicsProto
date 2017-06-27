#pragma once

#include <string>

#include "SGGEntityHandler.h"

class SGGScene
{
private:

	SGGEntityHandler* entHandler;

public:
	SGGScene(EntitySortingSettings& sortingSettings, MeshHandler* mesh, ShaderHandler* pipeline, LightHandler* light, CameraHandler* camera, MaterialHandler* material, TransformHandler* transform);
	~SGGScene();

	SGGEntityHandler* Entity();

};


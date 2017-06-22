#pragma once

#include <string>

class SGGScene
{
private:

	SGGEntityHandler* entHandler;

public:
	SGGScene();
	~SGGScene();

	SGGEntityHandler* Entity();

	MeshHandler* Mesh();
	PipelineHandler* Pipeline();
	LightHandler* Light();
	CameraHandler* Camera();
	MaterialHandler* Material();
	TransformHandler* Transform();

};


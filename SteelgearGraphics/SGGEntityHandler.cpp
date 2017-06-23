#include "SGGEntityHandler.h"

SGGEntityHandler::SGGEntityHandler(MeshHandler * mesh, ShaderHandler * pipeline, LightHandler * light, CameraHandler * camera, MaterialHandler * material, TransformHandler * transform)
{
	meshHandler = mesh;
	pipelineHandler = pipeline;
	lightHandler = light;
	cameraHandler = camera;
	materialHandler = material;
	transformHandler = transform;
}

SGGEntityHandler::~SGGEntityHandler()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (entities[i]->meshID != -1)
		{
			//meshHandler->RemoveMesh(entities[i]);
		}

		if (entities[i]->lightID != -1)
		{
			//lightHandler->RemoveLight(entities[i]);
		}

		if (entities[i]->cameraID != -1)
		{
			//cameraHandler->RemoveCamera(entities[i]);
		}

		if (entities[i]->materialsIDs.diffuseTex != 0)
		{
			//materialHandler->RemoveMaterials(entities[i], MaterialType::diffuse);
		}

		if (entities[i]->materialsIDs.roughnessTex != 1)
		{
			//materialHandler->RemoveMaterials(entities[i], MaterialType::roughness);
		}

		if (entities[i]->materialsIDs.metallicTex != 2)
		{
			//materialHandler->RemoveMaterials(entities[i], MaterialType::metallic);
		}

		if (entities[i]->materialsIDs.normalMap != 3)
		{
			//materialHandler->RemoveMaterials(entities[i], MaterialType::normal);
		}

		if (entities[i]->materialsIDs.displacementMap != 4)
		{
			//materialHandler->RemoveMaterials(entities[i], MaterialType::displacement);
		}

		if (entities[i]->shaderID != -1)
		{
			//pipelineHandler->RemoveShaders(entities[i]);
		}

		delete entities[i];
	}
}

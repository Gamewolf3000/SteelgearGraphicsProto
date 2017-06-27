#include "SGGEntityHandler.h"

const SortedEntitiesData & SGGEntityHandler::GetEntities()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (entities[i]->needsSorting)
		{
			entSorter->UpdateEntity(entities[i]);
		}
	}

	return entSorter->GetSortedEnteties();
}

SGGEntityHandler::SGGEntityHandler(EntitySortingSettings& sortingSettings, MeshHandler * mesh, ShaderHandler * pipeline, LightHandler * light, CameraHandler * camera, MaterialHandler * material, TransformHandler * transform)
{
	meshHandler = mesh;
	pipelineHandler = pipeline;
	lightHandler = light;
	cameraHandler = camera;
	materialHandler = material;
	transformHandler = transform;

	entSorter = new EntitySorter(sortingSettings);
}

SGGEntityHandler::~SGGEntityHandler()
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (entities[i]->meshID != -1)
		{
			meshHandler->RemoveMesh(*entities[i]);
		}

		if (entities[i]->lightID != -1)
		{
			lightHandler->RemoveLight(*entities[i]);
		}

		if (entities[i]->cameraID != -1)
		{
			cameraHandler->RemoveCamera(*entities[i]);
		}

		if (entities[i]->materialsIDs.diffuseTex != 0)
		{
			materialHandler->RemoveMaterials(*entities[i], MaterialType::diffuse);
		}

		if (entities[i]->materialsIDs.roughnessTex != 1)
		{
			materialHandler->RemoveMaterials(*entities[i], MaterialType::roughness);
		}

		if (entities[i]->materialsIDs.metallicTex != 2)
		{
			materialHandler->RemoveMaterials(*entities[i], MaterialType::metallic);
		}

		if (entities[i]->materialsIDs.normalMap != 3)
		{
			materialHandler->RemoveMaterials(*entities[i], MaterialType::normal);
		}

		if (entities[i]->materialsIDs.displacementMap != 4)
		{
			materialHandler->RemoveMaterials(*entities[i], MaterialType::displacement);
		}

		if (entities[i]->shaderID != -1)
		{
			pipelineHandler->RemoveShaders(*entities[i]);
		}

		delete entities[i];
	}

	delete entSorter;
}

SGGEntity * SGGEntityHandler::CreateEntity()
{
	SGGEntity* temp = new SGGEntity;

	if (freeSpots.size() > 0)
	{
		entities[freeSpots[freeSpots.size() - 1]] = temp;
		temp->idNumber = freeSpots[freeSpots.size() - 1];

		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
	}
	else
	{
		entities.push_back(temp);
		temp->idNumber = entities.size() - 1;
	}

	entSorter->InsertEntity(temp);

	return temp;
}

SGGEntity * SGGEntityHandler::CreateEntity(std::string identifier)
{
	SGGEntity* temp = new SGGEntity;

	if (freeSpots.size() > 0)
	{
		entities[freeSpots[freeSpots.size() - 1]] = temp;
		temp->idNumber = freeSpots[freeSpots.size() - 1];

		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
	}
	else
	{
		entities.push_back(temp);
		temp->idNumber = entities.size() - 1;
	}

	temp->identifier = identifier;

	entSorter->InsertEntity(temp);

	return temp;
}

void SGGEntityHandler::DeleteEntity(SGGEntity & entity)
{
	entity.transform = TransformData();

	if (entity.cameraID != -1)
	{
		cameraHandler->RemoveCamera(entity);
	}

	if (entity.meshID != -1)
	{
		meshHandler->RemoveMesh(entity);
	}

	if (entity.shaderID != -1)
	{
		pipelineHandler->RemoveShaders(entity);
	}

	materialHandler->RemoveMaterials(entity, diffuse);
	materialHandler->RemoveMaterials(entity, roughness);
	materialHandler->RemoveMaterials(entity, metallic);
	materialHandler->RemoveMaterials(entity, normal);
	materialHandler->RemoveMaterials(entity, displacement);

	unsigned int idNr = entity.idNumber;
	delete entities[idNr];
	entities[idNr] = nullptr;
	entities.erase(entities.begin() + idNr);
	entities.shrink_to_fit();

}

TransformHandler* SGGEntityHandler::Transform()
{
	return transformHandler;
}

MeshHandler * SGGEntityHandler::Mesh()
{
	return meshHandler;
}

ShaderHandler * SGGEntityHandler::Shader()
{
	return pipelineHandler;
}

CameraHandler * SGGEntityHandler::Camera()
{
	return cameraHandler;
}

MaterialHandler * SGGEntityHandler::Material()
{
	return materialHandler;
}

LightHandler * SGGEntityHandler::Light()
{
	return lightHandler;
}
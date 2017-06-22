#include "MeshHandler.h"
#include "EntityHandler.h"

MeshHandler::MeshHandler()
{
	
}

MeshHandler::MeshHandler(wchar_t directory[200])
{
	objLoader = new OBJLoader(directory);
}

MeshHandler::~MeshHandler()
{
	for (auto& i : entityMeshes)
	{
		delete i.vertexBuffer;
		delete i.indexBuffer;
	}

	delete objLoader;
}

void MeshHandler::ClearJobVector()
{
	meshJobs.clear();
}

MeshData MeshHandler::LoadOBJ(std::string fileName)
{
	MeshData temp;
	temp = objLoader->LoadOBJFile(fileName);

	return temp;
}

void MeshHandler::BindMesh(Entity& entity, std::string name)
{

	for (unsigned int i = 0; i < entityMeshes.size(); i++)
	{
		if (name == entityMeshes[i].identifier && entityMeshes[i].nrOfUsers > 0) // Look for an already loaded mesh
		{
			entity.meshID = i;
			entityMeshes[i].nrOfUsers++;
			return;
		}
	}

	if (freeSpots.size() > 0)
	{
		entity.meshID = freeSpots[freeSpots.size() - 1];
		freeSpots.pop_back();
		freeSpots.shrink_to_fit();

		entityMeshes[entity.meshID] = LoadOBJ(name);
	}
	else
	{
		entityMeshes.push_back(LoadOBJ(name));
		entity.meshID = entityMeshes.size() - 1;
	}

	entityMeshes[entity.meshID].identifier = name;
	entityMeshes[entity.meshID].nrOfUsers = 1;
	meshJobs.push_back(entityMeshes[entity.meshID]);
}

void MeshHandler::BindMesh(Entity & entity, MeshData mesh)
{

	if (freeSpots.size() > 0)
	{
		entity.meshID = freeSpots[freeSpots.size() - 1];
		freeSpots.pop_back();
		freeSpots.shrink_to_fit();

		entityMeshes[entity.meshID] = mesh;
	}
	else
	{
		entityMeshes.push_back(mesh);
		entity.meshID = entityMeshes.size() - 1;
	}

	entityMeshes[entity.meshID].nrOfUsers = 1;
	meshJobs.push_back(entityMeshes[entity.meshID]);
}

void MeshHandler::RemoveMesh(Entity & entity)
{
	entityMeshes[entity.meshID].nrOfUsers--;

	if (entityMeshes[entity.meshID].nrOfUsers <= 0)
	{
		delete entityMeshes[entity.meshID].vertexBuffer;
		delete entityMeshes[entity.meshID].indexBuffer;
		entityMeshes[entity.meshID].vertexBuffer = nullptr;
		entityMeshes[entity.meshID].indexBuffer = nullptr;

		entityMeshes[entity.meshID].nrOfIndices = 0;
		entityMeshes[entity.meshID].render = false;
		entityMeshes[entity.meshID].nrOfUsers = 0;

		meshJobs.push_back(entityMeshes[entity.meshID]);
		freeSpots.push_back(entity.meshID);

		entityMeshes[entity.meshID].identifier = "";
	}

	entity.meshID = -1;
}

std::vector<MeshData>& MeshHandler::GetMeshJobs()
{
	return meshJobs;
}

unsigned int MeshHandler::GetNrOfIndicesOfMesh(Entity & entity)
{
	return entityMeshes[entity.meshID].nrOfIndices;
}

bool MeshHandler::GetRender(Entity & entity)
{
	return entityMeshes[entity.meshID].render;
}

void MeshHandler::SetRender(Entity & entity, bool value)
{
	entityMeshes[entity.meshID].render = value;
}

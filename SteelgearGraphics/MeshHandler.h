#ifndef MESH_HANDLER_H
#define MESH_HANDLER_H
#pragma once

#include "EngineIncludes.h"
#include "OBJLoader.h"

struct Entity;

// Create a mesh handler class which loads and keeps tracks of all mesh data, request obj loading from it and let it check by the identifier if there is already a mesh data loaded, if there is
// then return it instead, don't let anything change the mesh data once it's loaded, nrOfUsers keeps track of how many entities are currently using it, if it reaches 0, delete it to clear up

class MeshHandler
{
	friend class EntityHandler;

private:
	std::vector<MeshData> entityMeshes;
	std::vector<int> freeSpots;

	std::vector<MeshData> meshJobs;
	std::vector<MeshData>& GetMeshJobs();
	void ClearJobVector();

	//BINLoader* binLoader
	OBJLoader* objLoader;


	
	MeshData LoadOBJ(std::string fileName);

public:

	MeshHandler();
	MeshHandler(wchar_t directory[200]);
	virtual ~MeshHandler();

	void BindMesh(Entity& entity, std::string name);
	void BindMesh(Entity& entity, MeshData mesh);
	void RemoveMesh(Entity& entity);

	unsigned int GetNrOfIndicesOfMesh(Entity& entity);

	bool GetRender(Entity& entity);
	void SetRender(Entity& entity, bool value);

};

#endif

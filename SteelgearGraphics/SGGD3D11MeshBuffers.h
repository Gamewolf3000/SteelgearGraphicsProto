#pragma once

#include <vector>

#include "SGGEngineIncludes.h"

#include "SGGD3D11Includes.h"

struct MeshBufferData
{
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
};

class SGGD3D11MeshBuffers
{
private:

	std::vector<MeshBufferData> meshes;
	std::vector<int> freeSpots;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

public:

	SGGD3D11MeshBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~SGGD3D11MeshBuffers();

	int CreateMeshBuffer(StandardVertex* vertices, unsigned int nrOfVertices, unsigned int* indices, unsigned int nrOfIndices);
	void RemoveMeshBuffer(int idOfBufferToRemove);
};


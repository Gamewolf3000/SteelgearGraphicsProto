#ifndef D3D_BUFFER_CLASS_H
#define D3D_BUFFER_CLASS_H
#pragma once

#include "EngineIncludes.h"
#include "D3D11Includes.h"

struct MeshBuffer
{
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	std::string id;
};

class D3D11BufferClass
{
private:
	std::vector<MeshBuffer> meshBuffers;
	std::vector<int> freeSpotsMeshes;

public:

	D3D11BufferClass();
	virtual ~D3D11BufferClass();

	void CreateMeshBuffers(MeshData& data, ID3D11Device* device);
	void RemoveMeshBuffers(std::string identifier);
	ID3D11Buffer** GetVertexBuffer(int meshID);
	ID3D11Buffer* GetIndexBuffer(int meshID);

};

#endif

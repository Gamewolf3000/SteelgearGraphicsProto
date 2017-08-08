#include "SGGD3D11MeshBuffers.h"



SGGD3D11MeshBuffers::SGGD3D11MeshBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
}


SGGD3D11MeshBuffers::~SGGD3D11MeshBuffers()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		SafeReleaseD3D(meshes[i].vertexBuffer);
		SafeReleaseD3D(meshes[i].indexBuffer);
	}
}

int SGGD3D11MeshBuffers::CreateMeshBuffer(StandardVertex* vertices, unsigned int nrOfVertices, unsigned int* indices, unsigned int nrOfIndices)
{
	unsigned int meshSlot = -1;

	if (freeSpots.size() != 0)
	{
		meshSlot = freeSpots[freeSpots.size() - 1];
		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
	}
	else
	{
		meshSlot = meshes.size();
		MeshBufferData temp;
		meshes.push_back(temp);
	}



	D3D11_BUFFER_DESC vBufferDesc;
	memset(&vBufferDesc, 0, sizeof(vBufferDesc));
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(StandardVertex) * nrOfVertices;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = vertices;
	device->CreateBuffer(&vBufferDesc, &vData, &meshes[meshSlot].vertexBuffer);



	D3D11_BUFFER_DESC iBufferDesc;
	memset(&iBufferDesc, 0, sizeof(iBufferDesc));
	iBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iBufferDesc.ByteWidth = sizeof(int) * nrOfIndices;
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	iBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA orderData;
	orderData.pSysMem = indices;
	device->CreateBuffer(&iBufferDesc, &orderData, &meshes[meshSlot].indexBuffer);



	return meshSlot;
}

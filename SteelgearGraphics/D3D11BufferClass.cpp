#include "D3D11BufferClass.h"

D3D11BufferClass::D3D11BufferClass()
{

}

D3D11BufferClass::~D3D11BufferClass()
{
	for (int i = 0; i < meshBuffers.size(); i++)
	{
		SafeReleaseD3D(meshBuffers[i].vertexBuffer);
		SafeReleaseD3D(meshBuffers[i].indexBuffer);
	}
}

void D3D11BufferClass::CreateMeshBuffers(MeshData& data, ID3D11Device * device)
{

	for (unsigned int i = 0; i < meshBuffers.size(); i++)
	{
		if (data.identifier == meshBuffers[i].id) // Look for already created buffers
		{
			return;
		}
	}

	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;

	D3D11_BUFFER_DESC vBufferDesc;
	memset(&vBufferDesc, 0, sizeof(vBufferDesc));
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(StandardVertex) * data.nrOfIndices;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = data.vertexBuffer;
	device->CreateBuffer(&vBufferDesc, &vData, &vBuffer);


	UINT* indices = new UINT[data.nrOfIndices];

	for (unsigned int i = 0; i < data.nrOfIndices; i++)
	{
		//indices[i] = data.nrOfIndices - 1 - i; // why?
		indices[i] = data.indexBuffer[i];
	}

	D3D11_BUFFER_DESC iBufferDesc;
	memset(&iBufferDesc, 0, sizeof(iBufferDesc));
	iBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iBufferDesc.ByteWidth = sizeof(int) * data.nrOfIndices;
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	iBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA orderData;
	orderData.pSysMem = indices;
	device->CreateBuffer(&iBufferDesc, &orderData, &iBuffer);
	delete indices;


	if (freeSpotsMeshes.size() > 0)
	{
		meshBuffers[freeSpotsMeshes[freeSpotsMeshes.size() - 1]].vertexBuffer = vBuffer;
		meshBuffers[freeSpotsMeshes[freeSpotsMeshes.size() - 1]].indexBuffer = iBuffer;
		meshBuffers[freeSpotsMeshes[freeSpotsMeshes.size() - 1]].id = data.identifier;

		freeSpotsMeshes.pop_back();
		freeSpotsMeshes.shrink_to_fit();
	}
	else
	{
		MeshBuffer temp;
		temp.vertexBuffer = vBuffer;
		temp.indexBuffer = iBuffer;
		temp.id = data.identifier;

		meshBuffers.push_back(temp);

	}
}

void D3D11BufferClass::RemoveMeshBuffers(std::string identifier)
{
	int meshID = -1;

	for (unsigned int i = 0; i < meshBuffers.size(); i++)
	{
		if (identifier == meshBuffers[i].id)
		{
			meshID = i;
			break;
		}
	}

	SafeReleaseD3D(meshBuffers[meshID].vertexBuffer);
	SafeReleaseD3D(meshBuffers[meshID].indexBuffer);
	meshBuffers[meshID].vertexBuffer = nullptr;
	meshBuffers[meshID].indexBuffer = nullptr;
	meshBuffers[meshID].id = "";

	freeSpotsMeshes.push_back(meshID);
}

ID3D11Buffer ** D3D11BufferClass::GetVertexBuffer(int meshID)
{
	return &meshBuffers[meshID].vertexBuffer;
}

ID3D11Buffer * D3D11BufferClass::GetIndexBuffer(int meshID)
{
	return meshBuffers[meshID].indexBuffer;
}

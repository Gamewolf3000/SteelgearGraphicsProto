#ifndef MODEL_IMPORTER_H
#define MODEL_IMPORTER_H


//#include "ClassIncludes.h"
#include <iostream>
#include <fstream>
#include <string>

#include "SGGMath.h"

#include "DirectXHelpers.h"
#include "pch.h"
#include "PlatformHelpers.h"
#include "WICTextureLoader.h"

using namespace std;

struct ObjVertexData
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
};

class ModelImporter
{
private:

	void setCounters(string filename);
	void setBuffers(ID3D11Device* gDevice);

	Float3D* vertices;
	Float2D* uvCoords;
	Float3D* normals;
	UINT* indices;
	ObjVertexData* modelData;

	int vCount, vtCount, vnCount, iCount;
	ID3D11ShaderResourceView* texture;

	ID3D11Buffer* modelVertexBuffer = nullptr;
	ID3D11Buffer* modelIndexBuffer = nullptr;

	wchar_t directory[200];
	string objName;

public:
	ModelImporter(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, string filename, wchar_t directoryPath[200]);
	virtual ~ModelImporter();

	ID3D11Buffer** getVBuffer();
	ID3D11Buffer* getIBuffer();
	int getNrOfIndices();
	string GetObjName();

	void loadMTL(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, string mtlname);
	ID3D11ShaderResourceView** getTexture();
	string getMtlName(string filename);

};

#endif
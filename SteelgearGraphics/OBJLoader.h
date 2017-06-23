#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H
#pragma once

#include <fstream>
#include <string>

#include "SGGEngineIncludes.h"
#include "SGGMath.h"

class OBJLoader
{
private:
	std::string* modelDirectory;

	Float3D* vertices = nullptr;
	Float2D* uvCoords = nullptr;
	Float3D* normals = nullptr;

	int vCount, vtCount, vnCount, iCount;

	void CountData(std::string fileName, MeshData& data);

public:

	OBJLoader(wchar_t directory[200]);
	virtual ~OBJLoader();

	MeshData LoadOBJFile(std::string fileName);

};

#endif

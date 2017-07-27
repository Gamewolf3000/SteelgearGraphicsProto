#pragma once

#include "SGGScene.h"
#include "SGGEngineIncludes.h"

class SGG3DAPI
{

protected:

	virtual void ThreadFunction() = 0;
	GraphicsSettings settings;

	SortedEntitiesData* dataPtr[2] = { nullptr, nullptr };
	unsigned int ptrSizes[2] = { 0, 0 };
	unsigned short freePtr = 0;

public:
	SGG3DAPI(GraphicsSettings gSettings);
	virtual ~SGG3DAPI();

	virtual void Stop() = 0;

	virtual void CopySceneData(SGGScene* scene) = 0;

};


#pragma once

#include "SGGScene.h"

class SGG3DAPI
{

private:

	virtual void ThreadFunction() = 0;

public:
	SGG3DAPI();
	virtual ~SGG3DAPI();

	virtual void Stop() = 0;

	virtual void CopySceneData(SGGScene* scene);


};


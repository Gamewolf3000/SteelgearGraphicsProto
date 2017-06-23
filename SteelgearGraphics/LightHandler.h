#ifndef LIGHT_HANDLER_CLASS_H
#define LIGHT_HANDLER_CLASS_H

#include <vector>

#include "SGGEngineIncludes.h"

class SGGEntity;
class TransformHandler;

class LightHandler
{
	friend class SGGEntityHandler;

private:
	std::vector<LightData> lights;
	std::vector<int> freeSpots;

	//TransformHandler* transformHandler;

public:
	LightHandler();
	virtual ~LightHandler();

	void BindPointLight(SGGEntity& entity, Vec offsetFromEntity, Vec colour, float range);
	void RemoveLight(SGGEntity& entity);

	bool IsActive(SGGEntity& entity);

};

#endif
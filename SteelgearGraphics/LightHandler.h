#ifndef LIGHT_HANDLER_CLASS_H
#define LIGHT_HANDLER_CLASS_H

#include "EngineIncludes.h"

struct Entity;
class TransformHandler;

class LightHandler
{
	friend class EntityHandler;

private:
	std::vector<LightData> lights;
	std::vector<int> freeSpots;

	//TransformHandler* transformHandler;

public:
	LightHandler();
	virtual ~LightHandler();

	void BindPointLight(Entity& entity, Vec offsetFromEntity, Vec colour, float range);
	void RemoveLight(Entity& entity);

	bool IsActive(Entity& entity);

};

#endif
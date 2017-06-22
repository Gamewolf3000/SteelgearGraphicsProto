#include "LightHandler.h"
#include "EntityHandler.h"

LightHandler::LightHandler()
{

}

LightHandler::~LightHandler()
{

}

void LightHandler::BindPointLight(Entity & entity, Vec offsetFromEntity, Vec colour, float range)
{
	int vectorSpot = -1;

	if (freeSpots.size() > 0)
	{
		vectorSpot = freeSpots[freeSpots.size() - 1];

		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
	}
	else
	{
		LightData temp;
		temp.typeOfLight = POINTLIGHT;
		temp.colour = Float3D(VecGetByIndex(0, colour), VecGetByIndex(1, colour), VecGetByIndex(2, colour));
		temp.offset = Float3D(VecGetByIndex(0, offsetFromEntity), VecGetByIndex(1, offsetFromEntity), VecGetByIndex(2, offsetFromEntity));
		temp.range = range;

		lights.push_back(temp);
		vectorSpot = lights.size() - 1;
	}

	entity.lightID = vectorSpot;

}

void LightHandler::RemoveLight(Entity & entity)
{
	LightData temp;
	lights[entity.lightID] = temp;
	freeSpots.push_back(entity.lightID);
	entity.lightID = -1;
}

bool LightHandler::IsActive(Entity & entity)
{
	return lights[entity.lightID].active;
}

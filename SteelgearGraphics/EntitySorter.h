#pragma once

#include <vector>

#include "SGGEntity.h"
#include "SGGEngineIncludes.h"

class EntitySorter
{
private:

	int FindEntity(int id);
	void InsertionSort(SGGEntity* start, unsigned int nrOfElements, unsigned short levelsSorted);

	EntitySortingSettings sortingSettings;
	SortedEntitiesData sortedEnteties;
	unsigned int sizeOfArr;

public:
	EntitySorter(EntitySortingSettings settings);
	~EntitySorter();

	const SortedEntitiesData& GetSortedEnteties();
	void InsertEntity(SGGEntity* entity);
	void UpdateEntity(SGGEntity* entity);

	

};
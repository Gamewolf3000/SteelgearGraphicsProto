#include "EntitySorter.h"

int EntitySorter::FindEntity(int id)
{
	for (int i = 0; i < sortedEnteties.nrOfEntities; i++)
	{
		if (sortedEnteties.arr[i].idNumber == id)
		{
			return i;
		}
	}
}

void EntitySorter::InsertionSort(SGGEntity* start, unsigned int nrOfElements, unsigned short levelsSorted)
{
	// zero or one element in list
	if (nrOfElements <= 1 || levelsSorted == 3)
		return;
	
	SGGEntity temp;
	int j = 0;

	if (sortingSettings.order[levelsSorted] == PIPELINE)
	{
		for (int i = 1; i < nrOfElements; i++)
		{
			temp = start[i];
			j = i - 1;

			while (j >= 0 && start[j].shaderID > temp.shaderID)
			{
				start[j + 1] = start[j];
				j--;
			}
			start[j + 1] = temp;

		}
	}
	else if (sortingSettings.order[levelsSorted] == MESH)
	{
		for (int i = 1; i < nrOfElements; i++)
		{
			temp = start[i];
			j = i - 1;

			while (j >= 0 && start[j].meshID > temp.meshID)
			{
				start[j + 1] = start[j];
				j--;
			}
			start[j + 1] = temp;

		}
	}
	else if (sortingSettings.order[levelsSorted] == MATERIALS)
	{
		for (int i = 1; i < nrOfElements; i++)
		{
			temp = start[i];
			j = i - 1;

			while (j >= 0 && start[j].materialsIDs > temp.materialsIDs)
			{
				start[j + 1] = start[j];
				j--;
			}
			start[j + 1] = temp;

		}
	}
	
	if (sortingSettings.order[levelsSorted] == PIPELINE)
	{
		SGGEntity* newStart = start;
		int newNrOf = 1;
		int currentPipeline = start[0].shaderID;
		int i = 1;

		while (i < nrOfElements)
		{
			if (currentPipeline != start[i].shaderID)
			{
				InsertionSort(newStart, newNrOf, levelsSorted + 1);
				newStart = start + newNrOf;
				newNrOf = 1;
			}
			else
			{
				newNrOf++;
			}

			i++;
		}

		InsertionSort(newStart, newNrOf, levelsSorted + 1);
	}
	else if (sortingSettings.order[levelsSorted] == MESH)
	{
		SGGEntity* newStart = start;
		int newNrOf = 1;
		int currentMesh = start[0].meshID;
		int i = 1;

		while (i < nrOfElements)
		{
			if (currentMesh != start[i].meshID)
			{
				InsertionSort(newStart, newNrOf, levelsSorted + 1);
				newStart = start + newNrOf;
				newNrOf = 1;
			}
			else
			{
				newNrOf++;
			}

			i++;
		}

		InsertionSort(newStart, newNrOf, levelsSorted + 1);
	}
	else if (sortingSettings.order[levelsSorted] == MATERIALS)
	{
		SGGEntity* newStart = start;
		int newNrOf = 1;
		SGGEntityMaterials currentMaterials = start[0].materialsIDs;
		int i = 1;

		while (i < nrOfElements)
		{
			if (!(currentMaterials == start[i].materialsIDs))
			{
				InsertionSort(newStart, newNrOf, levelsSorted + 1);
				newStart = start + newNrOf;
				newNrOf = 1;
			}
			else
			{
				newNrOf++;
			}

			i++;
		}

		InsertionSort(newStart, newNrOf, levelsSorted + 1);
	}
}

EntitySorter::EntitySorter(EntitySortingSettings settings)
{
	sortingSettings = settings;

	sortedEnteties.nrOfEntities = 0;
	sortedEnteties.arr = new SGGEntity[settings.incrementAmount];

	sizeOfArr = settings.incrementAmount;
}

EntitySorter::~EntitySorter()
{
	delete sortedEnteties.arr;
}

const SortedEntitiesData & EntitySorter::GetSortedEnteties()
{
	InsertionSort(sortedEnteties.arr, sortedEnteties.nrOfEntities, 0);

	return sortedEnteties;
}

void EntitySorter::InsertEntity(SGGEntity * entity)
{
	if (sortedEnteties.nrOfEntities == sizeOfArr)
	{
		sizeOfArr += sortingSettings.incrementAmount;
		SGGEntity* newArr = new SGGEntity[sizeOfArr];

		for (int i = 0; i < sortedEnteties.nrOfEntities; i++)
		{
			newArr[i] = sortedEnteties.arr[i];
		}

		delete sortedEnteties.arr;
		sortedEnteties.arr = newArr;
	}

	sortedEnteties.arr[sortedEnteties.nrOfEntities] = *entity;
	sortedEnteties.nrOfEntities++;
}

void EntitySorter::UpdateEntity(SGGEntity * entity)
{
	int index = FindEntity(entity->idNumber);
	sortedEnteties.arr[index] = *entity;
}

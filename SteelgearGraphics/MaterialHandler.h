#ifndef MATERIAL_HANDLER_H
#define MATERIAL_HANDLER_H
#pragma once

#include "EngineIncludes.h"

struct Entity;

class MaterialHandler
{
	friend class EntityHandler;

private:
	std::vector<MaterialData> materials;
	std::vector<int> freeSpots;

	std::vector<MaterialData> materialJobs;
	std::vector<MaterialData>& GetMaterialJobs();
	void ClearJobVector();

public:

	MaterialHandler();
	virtual ~MaterialHandler();

	void BindMaterial(Entity& entity, std::string fileName, MaterialType type, unsigned int nrOfFrames = 1);
	void RemoveMaterials(Entity& entity, MaterialType type);

	void Animate(Entity& entity, MaterialType type);
	void AnimateReverse(Entity& entity, MaterialType type);
	int GetAnimationFrame(Entity& entity, MaterialType type);
};

#endif

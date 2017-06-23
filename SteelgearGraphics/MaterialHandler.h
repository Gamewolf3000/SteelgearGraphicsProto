#ifndef MATERIAL_HANDLER_H
#define MATERIAL_HANDLER_H
#pragma once

#include <vector>

#include "SGGEngineIncludes.h"

class SGGEntity;

class MaterialHandler
{
	friend class SGGEntityHandler;

private:
	std::vector<MaterialData> materials;
	std::vector<int> freeSpots;

	std::vector<MaterialData> materialJobs;
	std::vector<MaterialData>& GetMaterialJobs();
	void ClearJobVector();

public:

	MaterialHandler();
	virtual ~MaterialHandler();

	void BindMaterial(SGGEntity& entity, std::string fileName, MaterialType type, unsigned int nrOfFrames = 1);
	void RemoveMaterials(SGGEntity& entity, MaterialType type);

	void Animate(SGGEntity& entity, MaterialType type);
	void AnimateReverse(SGGEntity& entity, MaterialType type);
	int GetAnimationFrame(SGGEntity& entity, MaterialType type);
};

#endif

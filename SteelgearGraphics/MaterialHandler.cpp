#include "MaterialHandler.h"
#include "SGGEntityHandler.h"
#include "SGGEntity.h"

std::vector<MaterialData>& MaterialHandler::GetMaterialJobs()
{
	return materialJobs;
}

void MaterialHandler::ClearJobVector()
{
	materialJobs.clear();
}

MaterialHandler::MaterialHandler()
{
	// Create default materials --------------------------------------------------------------------------

	MaterialData temp;
	unsigned int vectorPos;

	temp.fileName = "DefaultTexture.png";
	vectorPos = 0;

	temp.type = diffuse;	
	temp.nrOfFrames = 1;
	temp.currentFrame = 0;
	temp.nrOfUsers = 99999999;

	materials.push_back(temp);
	materialJobs.push_back(materials[vectorPos]);


	temp.fileName = "DefaultRoughness.png";
	vectorPos = 1;

	temp.type = roughness;
	temp.nrOfFrames = 1;
	temp.currentFrame = 0;
	temp.nrOfUsers = 99999999;

	materials.push_back(temp);
	materialJobs.push_back(materials[vectorPos]);


	temp.fileName = "DefaultMetallic.png";
	vectorPos = 2;

	temp.type = metallic;
	temp.nrOfFrames = 1;
	temp.currentFrame = 0;
	temp.nrOfUsers = 99999999;

	materials.push_back(temp);
	materialJobs.push_back(materials[vectorPos]);


	temp.fileName = "DefaultNormalMap.png";
	vectorPos = 3;

	temp.type = normal;
	temp.nrOfFrames = 1;
	temp.currentFrame = 0;
	temp.nrOfUsers = 99999999;

	materials.push_back(temp);
	materialJobs.push_back(materials[vectorPos]);


	temp.fileName = "DefaultDisplacementMap.png";
	vectorPos = 4;

	temp.type = displacement;
	temp.nrOfFrames = 1;
	temp.currentFrame = 0;
	temp.nrOfUsers = 99999999;

	materials.push_back(temp);
	materialJobs.push_back(materials[vectorPos]);
}

MaterialHandler::~MaterialHandler()
{
}

void MaterialHandler::BindMaterial(SGGEntity & entity, std::string fileName, MaterialType type, unsigned int nrOfFrames)
{
	if (nrOfFrames > 1)
	{
		std::string remadeName = fileName.substr(0, fileName.size() - 4) + "0" + fileName.substr(fileName.size() - 4, fileName.size() - 1);
		for (unsigned int i = 0; i < materials.size(); i++)
		{
			if (remadeName == materials[i].fileName && materials[i].nrOfUsers > 0) // Look for an already loaded material
			{

				if (type == diffuse)
				{
					entity.materialsIDs.diffuseTex = i;
					entity.materialsIDs.UpdateComparator();
				}
				else if (type == roughness)
				{
					entity.materialsIDs.roughnessTex = i;
					entity.materialsIDs.UpdateComparator();
				}
				else if (type == metallic)
				{
					entity.materialsIDs.metallicTex = i;
					entity.materialsIDs.UpdateComparator();
				}
				else if (type == normal)
				{
					entity.materialsIDs.normalMap = i;
					entity.materialsIDs.UpdateComparator();
				}
				else if (type == displacement)
				{
					entity.materialsIDs.displacementMap = i;
					entity.materialsIDs.UpdateComparator();
				}

				materials[i].nrOfUsers++;
				return;
			}
		}

		if (type == diffuse)
		{
			entity.materialsIDs.diffuseTex = materials.size();
			entity.materialsIDs.UpdateComparator();
		}
		else if (type == roughness)
		{
			entity.materialsIDs.roughnessTex = materials.size();
			entity.materialsIDs.UpdateComparator();
		}
		else if (type == metallic)
		{
			entity.materialsIDs.metallicTex = materials.size();
			entity.materialsIDs.UpdateComparator();
		}
		else if (type == normal)
		{
			entity.materialsIDs.normalMap = materials.size();
			entity.materialsIDs.UpdateComparator();
		}
		else if (type == displacement)
		{
			entity.materialsIDs.displacementMap = materials.size();
			entity.materialsIDs.UpdateComparator();
		}

		for (int i = 0; i < nrOfFrames; i++)
		{
			MaterialData temp;
			unsigned int vectorPos = materials.size();
			temp.fileName= fileName.substr(0, fileName.size() - 4) + ::std::to_string(i) + fileName.substr(fileName.size() - 4, fileName.size() - 1);
			temp.type = type;

			temp.nrOfFrames = nrOfFrames;
			temp.currentFrame = i;
			temp.nrOfUsers = 1;

			materials.push_back(temp);
			materialJobs.push_back(temp);
		}

		return;
	}
	else
	{
		for (unsigned int i = 0; i < materials.size(); i++)
		{
			if (fileName == materials[i].fileName && materials[i].nrOfUsers > 0) // Look for an already loaded material
			{

				if (type == diffuse)
				{
					entity.materialsIDs.diffuseTex = i;
					entity.materialsIDs.UpdateComparator();
				}
				else if (type == roughness)
				{
					entity.materialsIDs.roughnessTex = i;
					entity.materialsIDs.UpdateComparator();
				}
				else if (type == metallic)
				{
					entity.materialsIDs.metallicTex = i;
					entity.materialsIDs.UpdateComparator();
				}
				else if (type == normal)
				{
					entity.materialsIDs.normalMap = i;
					entity.materialsIDs.UpdateComparator();
				}
				else if (type == displacement)
				{
					entity.materialsIDs.displacementMap = i;
					entity.materialsIDs.UpdateComparator();
				}

				materials[i].nrOfUsers++;
				return;
			}
		}

		unsigned int vectorPos = -1;

		if (freeSpots.size() > 0)
		{
			MaterialData temp;
			temp.fileName = fileName;
			vectorPos = freeSpots[freeSpots.size() - 1];
			freeSpots.pop_back();
			freeSpots.shrink_to_fit();

			if (type == diffuse)
			{
				entity.materialsIDs.diffuseTex = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = diffuse;
			}
			else if (type == roughness)
			{
				entity.materialsIDs.roughnessTex = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = roughness;
			}
			else if (type == metallic)
			{
				entity.materialsIDs.metallicTex = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = metallic;
			}
			else if (type == normal)
			{
				entity.materialsIDs.normalMap = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = normal;
			}
			else if (type == displacement)
			{
				entity.materialsIDs.displacementMap = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = displacement;
			}

			temp.nrOfFrames = 1;
			temp.currentFrame = 0;
			temp.nrOfUsers = 1;
		}
		else
		{
			MaterialData temp;
			temp.fileName = fileName;
			vectorPos = materials.size();

			if (type == diffuse)
			{
				entity.materialsIDs.diffuseTex = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = diffuse;
			}
			else if (type == roughness)
			{
				entity.materialsIDs.roughnessTex = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = roughness;
			}
			else if (type == metallic)
			{
				entity.materialsIDs.metallicTex = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = metallic;
			}
			else if (type == normal)
			{
				entity.materialsIDs.normalMap = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = normal;
			}
			else if (type == displacement)
			{
				entity.materialsIDs.displacementMap = vectorPos;
				entity.materialsIDs.UpdateComparator();
				temp.type = displacement;
			}

			temp.nrOfFrames = 1;
			temp.currentFrame = 0;
			temp.nrOfUsers = 1;

			materials.push_back(temp);
		}

		materialJobs.push_back(materials[vectorPos]);
	}

	entity.needsSorting = true;
}

void MaterialHandler::RemoveMaterials(SGGEntity & entity, MaterialType type)
{
	int startFramePos = -1;
	if (type == diffuse)
	{
		startFramePos = entity.materialsIDs.diffuseTex - materials[entity.materialsIDs.diffuseTex].currentFrame;
		materials[startFramePos].nrOfUsers--;

		if (materials[startFramePos].nrOfUsers <= 0)
		{
			int iterator = materials[startFramePos].nrOfFrames;

			for (int i = 0; i < iterator; i++)
			{
				materials[startFramePos + i].currentFrame = 0;
				materials[startFramePos + i].nrOfFrames = 1;
				materials[startFramePos + i].nrOfUsers = 0;

				materialJobs.push_back(materials[startFramePos + i]);
				freeSpots.push_back(startFramePos + i);
				materials[startFramePos + i].fileName = "";
			}
		}

		entity.materialsIDs.diffuseTex = 0;
	}
	else if (type == roughness)
	{
		startFramePos = entity.materialsIDs.roughnessTex - materials[entity.materialsIDs.roughnessTex].currentFrame;
		materials[startFramePos].nrOfUsers--;

		if (materials[startFramePos].nrOfUsers <= 0)
		{
			int iterator = materials[startFramePos].nrOfFrames;

			for (int i = 0; i < iterator; i++)
			{
				materials[startFramePos + i].currentFrame = 0;
				materials[startFramePos + i].nrOfFrames = 1;
				materials[startFramePos + i].nrOfUsers = 0;

				materialJobs.push_back(materials[startFramePos + i]);
				freeSpots.push_back(startFramePos + i);
				materials[startFramePos + i].fileName = "";
			}
		}

		entity.materialsIDs.roughnessTex = 1;
	}
	else if (type == metallic)
	{
		startFramePos = entity.materialsIDs.metallicTex - materials[entity.materialsIDs.metallicTex].currentFrame;
		materials[startFramePos].nrOfUsers--;

		if (materials[startFramePos].nrOfUsers <= 0)
		{
			int iterator = materials[startFramePos].nrOfFrames;

			for (int i = 0; i < iterator; i++)
			{
				materials[startFramePos + i].currentFrame = 0;
				materials[startFramePos + i].nrOfFrames = 1;
				materials[startFramePos + i].nrOfUsers = 0;

				materialJobs.push_back(materials[startFramePos + i]);
				freeSpots.push_back(startFramePos + i);
				materials[startFramePos + i].fileName = "";
			}
		}

		entity.materialsIDs.metallicTex = 2;
	}
	else if (type == normal)
	{
		startFramePos = entity.materialsIDs.normalMap - materials[entity.materialsIDs.normalMap].currentFrame;
		materials[startFramePos].nrOfUsers--;

		if (materials[startFramePos].nrOfUsers <= 0)
		{
			int iterator = materials[startFramePos].nrOfFrames;

			for (int i = 0; i < iterator; i++)
			{
				materials[startFramePos + i].currentFrame = 0;
				materials[startFramePos + i].nrOfFrames = 1;
				materials[startFramePos + i].nrOfUsers = 0;

				materialJobs.push_back(materials[startFramePos + i]);
				freeSpots.push_back(startFramePos + i);
				materials[startFramePos + i].fileName = "";
			}
		}

		entity.materialsIDs.normalMap = 3;
	}
	else if (type == displacement)
	{
		startFramePos = entity.materialsIDs.displacementMap - materials[entity.materialsIDs.displacementMap].currentFrame;
		materials[startFramePos].nrOfUsers--;

		if (materials[startFramePos].nrOfUsers <= 0)
		{
			int iterator = materials[startFramePos].nrOfFrames;

			for (int i = 0; i < iterator; i++)
			{
				materials[startFramePos + i].currentFrame = 0;
				materials[startFramePos + i].nrOfFrames = 1;
				materials[startFramePos + i].nrOfUsers = 0;

				materialJobs.push_back(materials[startFramePos + i]);
				freeSpots.push_back(startFramePos + i);
				materials[startFramePos + i].fileName = "";
			}
		}

		entity.materialsIDs.displacementMap = 4;
	}

	entity.materialsIDs.UpdateComparator();
	entity.needsSorting = true;
}

void MaterialHandler::Animate(SGGEntity & entity, MaterialType type)
{
	if (type == diffuse)
	{
		if (materials[entity.materialsIDs.diffuseTex].currentFrame == materials[entity.materialsIDs.diffuseTex].nrOfFrames - 1)
		{
			entity.materialsIDs.diffuseTex -= (materials[entity.materialsIDs.diffuseTex].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.diffuseTex++;
		}
	}
	else if (type == roughness)
	{
		if (materials[entity.materialsIDs.roughnessTex].currentFrame == materials[entity.materialsIDs.roughnessTex].nrOfFrames - 1)
		{
			entity.materialsIDs.roughnessTex -= (materials[entity.materialsIDs.roughnessTex].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.roughnessTex++;
		}
	}
	else if (type == metallic)
	{
		if (materials[entity.materialsIDs.metallicTex].currentFrame == materials[entity.materialsIDs.metallicTex].nrOfFrames - 1)
		{
			entity.materialsIDs.metallicTex -= (materials[entity.materialsIDs.metallicTex].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.metallicTex++;
		}
	}
	else if (type == normal)
	{
		if (materials[entity.materialsIDs.normalMap].currentFrame == materials[entity.materialsIDs.normalMap].nrOfFrames - 1)
		{
			entity.materialsIDs.normalMap -= (materials[entity.materialsIDs.normalMap].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.normalMap++;
		}
	}
	else if (type == displacement)
	{
		if (materials[entity.materialsIDs.displacementMap].currentFrame == materials[entity.materialsIDs.displacementMap].nrOfFrames - 1)
		{
			entity.materialsIDs.displacementMap -= (materials[entity.materialsIDs.displacementMap].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.displacementMap++;
		}
	}

	entity.materialsIDs.UpdateComparator();
	entity.needsSorting = true;
}

void MaterialHandler::AnimateReverse(SGGEntity & entity, MaterialType type)
{
	if (type == diffuse)
	{
		if (materials[entity.materialsIDs.diffuseTex].currentFrame == 0)
		{
			entity.materialsIDs.diffuseTex += (materials[entity.materialsIDs.diffuseTex].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.diffuseTex--;
		}
	}
	else if (type == roughness)
	{
		if (materials[entity.materialsIDs.roughnessTex].currentFrame == 0)
		{
			entity.materialsIDs.roughnessTex += (materials[entity.materialsIDs.roughnessTex].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.roughnessTex--;
		}
	}
	else if (type == metallic)
	{
		if (materials[entity.materialsIDs.metallicTex].currentFrame == 0)
		{
			entity.materialsIDs.metallicTex += (materials[entity.materialsIDs.metallicTex].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.metallicTex--;
		}
	}
	else if (type == normal)
	{
		if (materials[entity.materialsIDs.normalMap].currentFrame == 0)
		{
			entity.materialsIDs.normalMap += (materials[entity.materialsIDs.normalMap].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.normalMap--;
		}
	}
	else if (type == displacement)
	{
		if (materials[entity.materialsIDs.displacementMap].currentFrame == 0)
		{
			entity.materialsIDs.displacementMap += (materials[entity.materialsIDs.displacementMap].nrOfFrames - 1);
		}
		else
		{
			entity.materialsIDs.displacementMap--;
		}
	}

	entity.materialsIDs.UpdateComparator();
	entity.needsSorting = true;
}

int MaterialHandler::GetAnimationFrame(SGGEntity & entity, MaterialType type)
{
	int returnValue = -1;

	if (type == diffuse)
	{
		returnValue = materials[entity.materialsIDs.diffuseTex].currentFrame;
	}
	else if (type == roughness)
	{
		returnValue = materials[entity.materialsIDs.roughnessTex].currentFrame;
	}
	else if (type == metallic)
	{
		returnValue = materials[entity.materialsIDs.metallicTex].currentFrame;
	}
	else if (type == normal)
	{
		returnValue = materials[entity.materialsIDs.normalMap].currentFrame;
	}
	else if (type == displacement)
	{
		returnValue = materials[entity.materialsIDs.displacementMap].currentFrame;
	}

	return returnValue;
}

#include "ShaderHandler.h"
#include "EntityHandler.h"

void ShaderHandler::ClearShaderJobs()
{
	shaderJobs.clear();
}

ShaderHandler::ShaderHandler(wchar_t directory[200])
{
	Entity dummyEnt;
	BindShaders(dummyEnt, "EnvironmentMapShader", true, false, false, false, true); // Bind the shaders for env map with cube maps

	Entity dummyEnt2;
	BindShaders(dummyEnt2, "IrradianceMapShader", true, false, false, false, true); // Bind the shaders for env map with cube maps
}

ShaderHandler::~ShaderHandler()
{
}

void ShaderHandler::BindShaders(Entity & entity, std::string shaderName, bool vS, bool hS, bool dS, bool gS, bool pS)
{
	for (unsigned int i = 0; i < shaders.size(); i++)
	{
		if (shaderName == shaders[i].identifier && shaders[i].nrOfUsers > 0) // Look for an already loaded shader
		{
			entity.shaderID = i;
			shaders[i].nrOfUsers++;
			return;
		}
	}

	if (freeSpots.size() > 0)
	{
		entity.shaderID = freeSpots[freeSpots.size() - 1];
		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
		shaders[entity.shaderID].nrOfUsers = 1;
		shaders[entity.shaderID].identifier = shaderName;

		shaders[entity.shaderID].vertexShader = vS;
		shaders[entity.shaderID].hullShader = hS;
		shaders[entity.shaderID].domainShader = dS;
		shaders[entity.shaderID].geometryShader = gS;
		shaders[entity.shaderID].pixelShader = pS;
	}
	else
	{
		ShaderData temp;
		temp.identifier = shaderName;
		temp.nrOfUsers = 1;

		temp.vertexShader = vS;
		temp.hullShader = hS;
		temp.domainShader = dS;
		temp.geometryShader = gS;
		temp.pixelShader = pS;

		shaders.push_back(temp);
		entity.shaderID = shaders.size() - 1;
	}

	shaderJobs.push_back(shaders[entity.shaderID]);

}

void ShaderHandler::RemoveShaders(Entity & entity)
{
	shaders[entity.shaderID].nrOfUsers--;

	if (shaders[entity.shaderID].nrOfUsers <= 0)
	{
		shaders[entity.shaderID].vertexShader = false;
		shaders[entity.shaderID].hullShader = false;
		shaders[entity.shaderID].domainShader = false;
		shaders[entity.shaderID].geometryShader = false;
		shaders[entity.shaderID].pixelShader = false;

		shaders[entity.shaderID].nrOfUsers = 0;

		shaderJobs.push_back(shaders[entity.shaderID]);
		freeSpots.push_back(entity.shaderID);

		shaders[entity.shaderID].identifier = "";
	}

	entity.shaderID = -1;
}

std::vector<ShaderData>& ShaderHandler::GetShaderJobs()
{
	return shaderJobs;
}

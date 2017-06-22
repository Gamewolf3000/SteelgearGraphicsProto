#ifndef SHADER_HANDLER_H
#define SHADER_HANDLER_H
#pragma once

#include "EngineIncludes.h"

struct Entity;

struct ShaderData
{
	bool vertexShader = false;
	bool hullShader = false;
	bool domainShader = false;
	bool geometryShader = false;
	bool pixelShader = false;

	unsigned int nrOfUsers = 0;
	std::string identifier;
};

class ShaderHandler
{
	friend class EntityHandler;

private:
	std::vector<ShaderData> shaders;
	std::vector<int> freeSpots;

	std::vector<ShaderData> shaderJobs;

	std::vector<ShaderData>& GetShaderJobs();
	void ClearShaderJobs();

public:

	ShaderHandler(wchar_t directory[200]);
	virtual ~ShaderHandler();

	void BindShaders(Entity& entity, std::string shaderName, bool vS, bool hS, bool dS, bool gS, bool pS);
	void RemoveShaders(Entity& entity);

};

#endif

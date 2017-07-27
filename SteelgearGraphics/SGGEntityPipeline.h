#pragma once

#include <vector>
#include <string>

#include "SGGEngineIncludes.h"

enum PipelineType
{
	RENDER,
	COMPUTE
};

enum PipelineUsage
{
	USED_FOR_EACH_ENTITY,
	USED_ONCE
};

enum DataUsage
{
	USED_PER_ENTITY,
	USED_GLOBALY,
	NOT_USED
};

struct ShaderData
{
	DataUsage usage = NOT_USED;
	int slot = TRANSFORMSLOT;
};

struct SGGPipeline
{
	PipelineType type = RENDER;
	PipelineUsage usage = USED_FOR_EACH_ENTITY;
};

struct SGGGraphicsPipeline : public SGGPipeline
{
	std::string vertexShader = "";
	ShaderData vertexConstantBuffers[NROFCONSTBUFFERSLOTS];
	//std::string hullShader = "";
	//std::string domainShader = "";
	std::string geometryShader = "";
	ShaderData geometryConstantBuffers[NROFCONSTBUFFERSLOTS];
	std::string pixelShader = "";
	ShaderData pixelConstantBuffers[NROFCONSTBUFFERSLOTS];
	ShaderData pixelTextures[NROFPIXELSHADERTEXTURESLOTS];
};

struct SGGComputePipeline : public SGGPipeline
{
	std::string computeShader = "";
	ShaderData computeConstantBuffers[NROFCONSTBUFFERSLOTS];
};

class SGGEntityPipeline
{
private:

	std::vector<SGGPipeline*> subPipelines;

public:
	SGGEntityPipeline();
	~SGGEntityPipeline();

	void AddNewPipeline(SGGPipeline* pipeline);
};


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

enum DataSource
{
	ENTITY,
	GLOBAL,
	RENDER_TARGET,
	NOT_USED
};

enum PrePipelineJobType
{
	CLEAR_RENDER_TARGET,
	CLEAR_DEPTH_STENCIL,
	SET_RENDER_TARGET_AND_DEPTH_STENCIL
};

struct ShaderData
{
	DataSource source = NOT_USED;
	int slot = TRANSFORMSLOT;
};

struct PrePipelineJob
{
	PrePipelineJobType type;
};

struct ClearRenderTarget : public PrePipelineJob
{
	unsigned short int rtSlot;
	Float4D clearColour;

	ClearRenderTarget(Float4D clearingColour, unsigned short int renderTargetSlot = 0)
	{
		type = CLEAR_RENDER_TARGET;
		rtSlot = renderTargetSlot;
		clearColour = clearingColour;
	}
};

struct ClearDepthStencil : public PrePipelineJob
{
	unsigned short int dsSlot;
	float clearDepth;

	ClearDepthStencil(float clearingDepth, unsigned short int depthStencilSlot = 0)
	{
		type = CLEAR_DEPTH_STENCIL;
		dsSlot = depthStencilSlot;
		clearDepth = clearingDepth;
	}
};

struct SetRenderTargetAndDepthStencil : public PrePipelineJob
{
	unsigned short int nrOfTargetsToSet;
	unsigned short int startSlotRender;
	unsigned short int depthStencilSlot;

	SetRenderTargetAndDepthStencil(unsigned short int depthStencilToSet, unsigned short int renderTargetStartingSlot = 0, unsigned short int nrOfRenderTargetsToSet = 1)
	{
		type = SET_RENDER_TARGET_AND_DEPTH_STENCIL;
		nrOfTargetsToSet = nrOfRenderTargetsToSet;
		startSlotRender = renderTargetStartingSlot;
		depthStencilSlot = depthStencilToSet;
	}
};

enum CullMode
{
	BACK_FACE,
	FRONT_FACE,
	NO_CULLING
};

enum TriangleDrawDirection
{
	CLOCKWISE,
	COUNTER_CLOCKWISE
};

struct RenderTarget
{
	unsigned int width;
	unsigned int height;
	bool usableAsShaderResource;
};

struct DepthStencil
{
	unsigned int width;
	unsigned int height;
	bool usableAsShaderResource;
};

struct BlendState
{

};

struct RasterizerState
{
	bool Wireframe;
	CullMode cullMode;
	TriangleDrawDirection triangleDrawDirection;
};

struct DepthStencilState
{

};

enum PostPipelineJobType
{
	PRESENT
};

struct PostPipelineJob
{
	PostPipelineJobType type;
};

struct Present : public PostPipelineJob
{
	Present()
	{
		type = PRESENT;
	}
};

struct SGGPipeline
{
	PipelineType type = RENDER;
	PipelineUsage usage = USED_FOR_EACH_ENTITY;
	std::vector<PrePipelineJob> prePipelineJobs;
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

	std::vector<RenderTarget> renderTargets;
	std::vector<DepthStencil> depthStencils;
	std::vector<BlendState> blendStates;
	std::vector<RasterizerState> rasterizerStates;
	std::vector<DepthStencilState> depthStencilStates;

	std::vector<PrePipelineJob*> prePipelineJobs;
	std::vector<PostPipelineJob*> postPipelineJobs;

	std::string identifier = "";

public:
	SGGEntityPipeline(std::string identifier);
	~SGGEntityPipeline();

	void AddNewPipeline(SGGPipeline* pipeline);
	void AddNewRenderTarget(unsigned int width, unsigned int height, bool usableAsShaderResource);
	void AddNewDepthStencil(unsigned int width, unsigned int height, bool usableAsShaderResource);
};


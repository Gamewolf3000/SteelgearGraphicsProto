#include "SGGEntityPipeline.h"

SGGEntityPipeline::SGGEntityPipeline(std::string identifier)
{
	this->identifier = identifier;
}


SGGEntityPipeline::~SGGEntityPipeline()
{
	for (int i = 0; i < subPipelines.size(); i++)
	{
		delete subPipelines[i];
	}
}

void SGGEntityPipeline::AddNewPipeline(SGGPipeline* pipeline)
{
	subPipelines.push_back(pipeline);
}

void SGGEntityPipeline::AddNewRenderTarget(unsigned int width, unsigned int height, bool usableAsShaderResource)
{
	RenderTarget temp;
	temp.width = width;
	temp.height = height;
	temp.usableAsShaderResource = usableAsShaderResource;

	renderTargets.push_back(temp);
}

void SGGEntityPipeline::AddNewDepthStencil(unsigned int width, unsigned int height)
{
	DepthStencil temp;
	temp.width = width;
	temp.height = height;

	depthStencils.push_back(temp);
}

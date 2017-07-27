#include "SGGEntityPipeline.h"



SGGEntityPipeline::SGGEntityPipeline()
{
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

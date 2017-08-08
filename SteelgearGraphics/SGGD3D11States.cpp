#include "SGGD3D11States.h"



SGGD3D11States::SGGD3D11States(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
}

SGGD3D11States::~SGGD3D11States()
{
	for (int i = 0; i < blend.size(); i++)
	{
		SafeReleaseD3D(blend[i]);
	}

	for (int i = 0; i < rasterizer.size(); i++)
	{
		SafeReleaseD3D(rasterizer[i]);
	}

	for (int i = 0; i < depth.size(); i++)
	{
		SafeReleaseD3D(depth[i]);
	}
}

int SGGD3D11States::CreateBlendState()
{
	unsigned int blendSlot = -1;

	if (freeSpotsbs.size() != 0)
	{
		blendSlot = freeSpotsbs[freeSpotsbs.size() - 1];
		freeSpotsbs.pop_back();
		freeSpotsbs.shrink_to_fit();
	}
	else
	{
		blendSlot = blend.size();
		ID3D11BlendState* temp;
		blend.push_back(temp);
	}

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendDesc, &blend[blendSlot]);

	return blendSlot;
}

void SGGD3D11States::RemoveBlendState(int idToRemove)
{
	SafeReleaseD3D(blend[idToRemove]);
	freeSpotsbs.push_back(idToRemove);
}

int SGGD3D11States::CreateRasterizerState()
{
	unsigned int rasterizerSlot = -1;

	if (freeSpotsrs.size() != 0)
	{
		rasterizerSlot = freeSpotsrs[freeSpotsrs.size() - 1];
		freeSpotsrs.pop_back();
		freeSpotsrs.shrink_to_fit();
	}
	else
	{
		rasterizerSlot = rasterizer.size();
		ID3D11RasterizerState* temp;
		rasterizer.push_back(temp);
	}

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	device->CreateRasterizerState(&rasterizerDesc, &rasterizer[rasterizerSlot]);

	return rasterizerSlot;
}

void SGGD3D11States::RemoveRasterizerState(int idToRemove)
{
	SafeReleaseD3D(rasterizer[idToRemove]);
	freeSpotsrs.push_back(idToRemove);
}

int SGGD3D11States::CreateDepthStencilState()
{
	unsigned int depthSlot = -1;

	if (freeSpotsdss.size() != 0)
	{
		depthSlot = freeSpotsdss[freeSpotsdss.size() - 1];
		freeSpotsdss.pop_back();
		freeSpotsdss.shrink_to_fit();
	}
	else
	{
		depthSlot = depth.size();
		ID3D11DepthStencilState* temp;
		depth.push_back(temp);
	}

	D3D11_DEPTH_STENCIL_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.StencilEnable = false;
	depthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&depthDesc, &depth[depthSlot]);

	return depthSlot;
}

void SGGD3D11States::RemoveDepthStencilState(int idToRemove)
{
	SafeReleaseD3D(depth[idToRemove]);
	freeSpotsdss.push_back(idToRemove);
}


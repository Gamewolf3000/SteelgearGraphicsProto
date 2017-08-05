#include "SGGD3D11RenderTargetAndDepthStencil.h"

SGGD3D11RenderTargetAndDepthStencil::SGGD3D11RenderTargetAndDepthStencil(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
}


SGGD3D11RenderTargetAndDepthStencil::~SGGD3D11RenderTargetAndDepthStencil()
{
	for (int i = 0; i < rtvs.size(); i++)
	{
		SafeReleaseD3D(rtvs[i].rtv);
		SafeReleaseD3D(rtvs[i].srv);
		SafeReleaseD3D(rtvs[i].texture);
	}
}

unsigned int SGGD3D11RenderTargetAndDepthStencil::CreateRenderTarget(unsigned int width, unsigned int height, bool usableAsShaderResource)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	unsigned int rtvSlot = -1;

	if (freeSpotsrt.size() != 0)
	{
		rtvSlot = freeSpotsrt[freeSpotsrt.size() - 1];
		freeSpotsrt.pop_back();
		freeSpotsrt.shrink_to_fit();
	}
	else
	{
		rtvSlot = rtvs.size();
		RTData temp;
		rtvs.push_back(temp);
	}

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	if (usableAsShaderResource)
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	else
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;


	// Create the render target texture
	result = device->CreateTexture2D(&textureDesc, NULL, &rtvs[rtvSlot].texture);

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target views.
	result = device->CreateRenderTargetView(rtvs[rtvSlot].texture, &renderTargetViewDesc, &rtvs[rtvSlot].rtv);

	if (usableAsShaderResource)
	{
		// Setup the description of the shader resource view.
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// Create the shader resource views.
		result = device->CreateShaderResourceView(rtvs[rtvSlot].texture, &shaderResourceViewDesc, &rtvs[rtvSlot].srv);
	}
	else
	{
		rtvs[rtvSlot].srv = nullptr;
	}


	return rtvSlot;
}

void SGGD3D11RenderTargetAndDepthStencil::RemoveRenderTarget(unsigned int idOfRenderTargetToRemove)
{
	SafeReleaseD3D(rtvs[idOfRenderTargetToRemove].rtv);
	SafeReleaseD3D(rtvs[idOfRenderTargetToRemove].srv);
	SafeReleaseD3D(rtvs[idOfRenderTargetToRemove].texture);
	freeSpotsrt.push_back(idOfRenderTargetToRemove);
}

unsigned int SGGD3D11RenderTargetAndDepthStencil::CreateDepthStencil(unsigned int width, unsigned int height, bool usableAsShaderResource)
{
	D3D11_TEXTURE2D_DESC textureDesc; // Create DepthStencil
	HRESULT result;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	unsigned int dsSlot = -1;

	if (freeSpotsds.size() != 0)
	{
		dsSlot = freeSpotsds[freeSpotsds.size() - 1];
		freeSpotsds.pop_back();
		freeSpotsds.shrink_to_fit();
	}
	else
	{
		dsSlot = dss.size();
		DSData temp;
		dss.push_back(temp);
	}

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	if (usableAsShaderResource)
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	else
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	device->CreateTexture2D(&textureDesc, 0, &dss[dsSlot].texture);

	depthStencilViewDesc.Format = textureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(dss[dsSlot].texture, 0, &dss[dsSlot].dsv);

	if (usableAsShaderResource)
	{
		// Setup the description of the shader resource view.
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// Create the shader resource views.
		result = device->CreateShaderResourceView(dss[dsSlot].texture, &shaderResourceViewDesc, &dss[dsSlot].srv);
	}
	else
	{
		dss[dsSlot].srv = nullptr;
	}

	return dsSlot;
}

void SGGD3D11RenderTargetAndDepthStencil::RemoveDepthStencil(unsigned int idOfDepthStencilToRemove)
{
	SafeReleaseD3D(dss[idOfDepthStencilToRemove].dsv);
	SafeReleaseD3D(dss[idOfDepthStencilToRemove].srv);
	SafeReleaseD3D(dss[idOfDepthStencilToRemove].texture);
	freeSpotsds.push_back(idOfDepthStencilToRemove);
}

void SGGD3D11RenderTargetAndDepthStencil::ClearRenderTarget(unsigned int idOfRenderTargetToClear, const Float4D & clearColour)
{
	float temp[4] = { clearColour.x, clearColour.y, clearColour.z, clearColour.w };
	deviceContext->ClearRenderTargetView(rtvs[idOfRenderTargetToClear].rtv, temp);
}

void SGGD3D11RenderTargetAndDepthStencil::ClearDepthStencil(unsigned int idOfDepthStencilToClear, float clearDepth)
{
	deviceContext->ClearDepthStencilView(dss[idOfDepthStencilToClear].dsv, D3D11_CLEAR_DEPTH, clearDepth, 0);
}

void SGGD3D11RenderTargetAndDepthStencil::SetRTDS(unsigned int rtStartId, unsigned short int nrOfRT, unsigned int dsId)
{
	ID3D11RenderTargetView* rts[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	for (int i = 0; i < nrOfRT; i++)
	{
		rts[i] = rtvs[rtStartId + i].rtv;
	}

	deviceContext->OMSetRenderTargets(nrOfRT, rts, dss[dsId].dsv);
}

void SGGD3D11RenderTargetAndDepthStencil::SetRTShaderResourceViewPS(unsigned short int slotToSet, unsigned int srvToSet)
{
	deviceContext->PSSetShaderResources(slotToSet, 1, &rtvs[srvToSet].srv);
}

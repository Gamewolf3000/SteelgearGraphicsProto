#include "SGGD3D11Shaders.h"

void SGGD3D11Shaders::CreateInputLayout(ID3D11Device* device, GraphicsShaderData & dataStruct, ID3DBlob* pVS)
{
	HRESULT hr;

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &dataStruct.inputLayout);
}

void SGGD3D11Shaders::LoadVertexShader(const std::string &name, GraphicsShaderData & dataStruct)
{
	HRESULT hr;
	std::wstring VSFilepath(name.begin(), name.end());

	//create vertex shader
	ID3DBlob* pVS = nullptr;
	
#ifdef _DEBUG
	hr = D3DCompileFromFile(VSFilepath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", D3DCOMPILE_DEBUG, NULL, &pVS, nullptr);
#else
	hr = D3DCompileFromFile(VSFilepath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", 0, NULL, &pVS, nullptr);
#endif

	hr = device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &dataStruct.vertexShader);

	CreateInputLayout(device, dataStruct, pVS);
	pVS->Release();
}

void SGGD3D11Shaders::LoadPixelShader(const std::string &name, GraphicsShaderData & dataStruct)
{
	HRESULT hr;
	std::wstring VSFilepath(name.begin(), name.end());

	//create vertex shader
	ID3DBlob* pPS = nullptr;

#ifdef _DEBUG
	hr = D3DCompileFromFile(VSFilepath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", D3DCOMPILE_DEBUG, NULL, &pPS, nullptr);
#else
	hr = D3DCompileFromFile(VSFilepath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", 0, NULL, &pPS, nullptr);
#endif

	hr = device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &dataStruct.pixelShader);

	pPS->Release();
}

SGGD3D11Shaders::SGGD3D11Shaders(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
}


SGGD3D11Shaders::~SGGD3D11Shaders()
{
}

int SGGD3D11Shaders::LoadGraphicsPipeline(const std::string & vs, const std::string & hs, const std::string & ds, const std::string & gs, const std::string & ps)
{
	unsigned int gpSlot = -1;

	if (freeSpotsGraphic.size() != 0)
	{
		gpSlot = freeSpotsGraphic[freeSpotsGraphic.size() - 1];
		freeSpotsGraphic.pop_back();
		freeSpotsGraphic.shrink_to_fit();
	}
	else
	{
		gpSlot = gShaders.size();
		GraphicsShaderData temp;
		gShaders.push_back(temp);
	}

	if (vs != "")
		LoadVertexShader(vs, gShaders[gpSlot]);
	//if (vs != "")
	//	LoadVertexShader(vs, gShaders[gpSlot]);
	//if (vs != "")
	//	LoadVertexShader(vs, gShaders[gpSlot]);
	//if (vs != "")
	//	LoadVertexShader(vs, gShaders[gpSlot]);
	if (ps != "")
		LoadPixelShader(ps, gShaders[gpSlot]);

	return gpSlot;
}

void SGGD3D11Shaders::RemoveGraphicsPipeline(unsigned int idOfGPToRemove)
{
	SafeReleaseD3D(gShaders[idOfGPToRemove].inputLayout);
	SafeReleaseD3D(gShaders[idOfGPToRemove].vertexShader);
	SafeReleaseD3D(gShaders[idOfGPToRemove].hullShader);
	SafeReleaseD3D(gShaders[idOfGPToRemove].domainShader);
	SafeReleaseD3D(gShaders[idOfGPToRemove].geometryShader);
	SafeReleaseD3D(gShaders[idOfGPToRemove].pixelShader);

	freeSpotsGraphic.push_back(idOfGPToRemove);
}

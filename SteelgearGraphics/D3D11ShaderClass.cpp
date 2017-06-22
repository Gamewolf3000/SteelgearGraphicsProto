#include "D3D11ShaderClass.h"

D3D11ShaderClass::D3D11ShaderClass(wchar_t directory[200])
{
	std::wstring temp(directory);
	shaderDirectory = new std::string(temp.begin(), temp.end());
}

D3D11ShaderClass::~D3D11ShaderClass()
{
	delete shaderDirectory;

	for (auto& i : shaders)
	{
		SafeReleaseD3D(i.inputLayout);
		SafeReleaseD3D(i.vertexShader);
		SafeReleaseD3D(i.domainShader);
		SafeReleaseD3D(i.hullShader);
		SafeReleaseD3D(i.geometryShader);
		SafeReleaseD3D(i.pixelShader);
	}
}

void D3D11ShaderClass::LoadShader(ShaderData data, ID3D11Device* device)
{
	int vectorSpot;

	for (unsigned int i = 0; i < shaders.size(); i++)
	{
		if (data.identifier == shaders[i].identifier) // Look for already loaded shaders
		{
			return;
		}
	}

	if (freeSpots.size() > 0)
	{
		vectorSpot = freeSpots[freeSpots.size() - 1];
		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
		shaders[vectorSpot].identifier = data.identifier;

		if(data.vertexShader)
			loadVertexShader(data.identifier, device, shaders[vectorSpot]);
		if(data.geometryShader)
			loadGeometryShader(data.identifier, device, shaders[vectorSpot]);
		if(data.pixelShader)
			loadPixelShader(data.identifier, device, shaders[vectorSpot]);
	}
	else
	{
		D3DShaderData temp;
		temp.identifier = data.identifier;

		if (data.vertexShader)
			loadVertexShader(data.identifier, device, temp);
		if (data.geometryShader)
			loadGeometryShader(data.identifier, device, temp);
		if (data.pixelShader)
			loadPixelShader(data.identifier, device, temp);

		shaders.push_back(temp);
	}

}

void D3D11ShaderClass::RemoveShader(std::string identifier)
{
	int shaderID = -1;

	for (unsigned int i = 0; i < shaders.size(); i++)
	{
		if (identifier == shaders[i].identifier)
		{
			shaderID = i;
			break;
		}
	}

	SafeReleaseD3D(shaders[shaderID].inputLayout);
	SafeReleaseD3D(shaders[shaderID].vertexShader);
	SafeReleaseD3D(shaders[shaderID].hullShader);
	SafeReleaseD3D(shaders[shaderID].domainShader);
	SafeReleaseD3D(shaders[shaderID].geometryShader);
	SafeReleaseD3D(shaders[shaderID].pixelShader);

	shaders[shaderID].identifier = "";

	freeSpots.push_back(shaderID);
}

void D3D11ShaderClass::SetShaders(int shaderID, ID3D11DeviceContext* deviceContext)
{
	deviceContext->VSSetShader(shaders[shaderID].vertexShader, nullptr, 0);
	deviceContext->HSSetShader(shaders[shaderID].hullShader, nullptr, 0);
	deviceContext->DSSetShader(shaders[shaderID].domainShader, nullptr, 0);
	deviceContext->GSSetShader(shaders[shaderID].geometryShader, nullptr, 0);
	deviceContext->PSSetShader(shaders[shaderID].pixelShader, nullptr, 0);
	deviceContext->IASetInputLayout(shaders[shaderID].inputLayout);
}

void D3D11ShaderClass::setInputLayout(ID3D11Device* device, D3DShaderData& dataStruct, ID3DBlob* pVS)
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

void D3D11ShaderClass::loadVertexShader(std::string name, ID3D11Device* device, D3DShaderData& dataStruct)
{
	HRESULT hr;
	wchar_t VSFilepath[250] = L"";
	std::wstring tempWDir(shaderDirectory->begin(), shaderDirectory->end());
	std::wstring tempWName(dataStruct.identifier.begin(), dataStruct.identifier.end());

	wcscat_s(VSFilepath, tempWDir.c_str());
	wcscat_s(VSFilepath, L"\\Shaders\\VertexShaders\\");
	wcscat_s(VSFilepath, tempWName.c_str());
	wcscat_s(VSFilepath, L"VS.hlsl");

	//create vertex shader
	ID3DBlob* pVS = nullptr;
	hr = D3DCompileFromFile(VSFilepath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", D3DCOMPILE_DEBUG, NULL, &pVS, nullptr);

	hr = device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &dataStruct.vertexShader);

	setInputLayout(device, dataStruct, pVS);
	pVS->Release();
}

void D3D11ShaderClass::loadGeometryShader(std::string name, ID3D11Device* device, D3DShaderData& dataStruct)
{
	// load geometry shader
}

void D3D11ShaderClass::loadPixelShader(std::string name, ID3D11Device* device, D3DShaderData& dataStruct)
{
	HRESULT hr;
	wchar_t PSFilepath[250] = L"";
	std::wstring tempWDir(shaderDirectory->begin(), shaderDirectory->end());
	std::wstring tempWName(dataStruct.identifier.begin(), dataStruct.identifier.end());

	wcscat_s(PSFilepath, tempWDir.c_str());
	wcscat_s(PSFilepath, L"\\Shaders\\PixelShaders\\");
	wcscat_s(PSFilepath, tempWName.c_str());
	wcscat_s(PSFilepath, L"PS.hlsl");

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	hr = D3DCompileFromFile(PSFilepath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", D3DCOMPILE_DEBUG, NULL, &pPS, nullptr);

	hr = device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &dataStruct.pixelShader);

	pPS->Release();
}
#ifndef D3D_SHADER_CLASS_H
#define D3D_SHADER_CLASS_H
#pragma once

#include "EngineIncludes.h"
#include "D3D11Includes.h"

#include "ShaderHandler.h"

class D3D11ShaderClass
{
private:
	std::vector<D3DShaderData> shaders;
	std::vector<int> freeSpots;
	std::string* shaderDirectory;

	void setInputLayout(ID3D11Device* device, D3DShaderData& dataStruct, ID3DBlob* pVS);
	void loadVertexShader(std::string name, ID3D11Device* device, D3DShaderData& dataStruct);
	void loadGeometryShader(std::string name, ID3D11Device* device, D3DShaderData& dataStruct);
	void loadPixelShader(std::string name, ID3D11Device* device, D3DShaderData& dataStruct);

public:

	D3D11ShaderClass(wchar_t directory[200]);
	virtual ~D3D11ShaderClass();

	void LoadShader(ShaderData data, ID3D11Device* device);
	void RemoveShader(std::string identifier);
	void SetShaders(int shaderID, ID3D11DeviceContext* deviceContext);


};

#endif

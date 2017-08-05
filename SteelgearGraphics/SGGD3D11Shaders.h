#pragma once

#include <vector>

#include "SGGEngineIncludes.h"

#include "SGGD3D11Includes.h"

struct GraphicsShaderData
{
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11HullShader* hullShader = nullptr;
	ID3D11DomainShader* domainShader = nullptr;
	ID3D11GeometryShader* geometryShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
};

struct ComputeShaderData
{

};

class SGGD3D11Shaders
{
private:

	std::vector<GraphicsShaderData> gShaders;
	std::vector<int> freeSpotsGraphic;
	std::vector<ComputeShaderData> cShaders;
	std::vector<int> freeSpotsCompute;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

	void CreateInputLayout(ID3D11Device * device, GraphicsShaderData & dataStruct, ID3DBlob * pVS);

	void LoadVertexShader(const std::string &name, GraphicsShaderData& dataStruct);
	void loadGeometryShader(const std::string &name, GraphicsShaderData& dataStruct);
	void LoadPixelShader(const std::string &name, GraphicsShaderData& dataStruct);

public:
	SGGD3D11Shaders(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~SGGD3D11Shaders();

	int LoadGraphicsPipeline(const std::string& vs, const std::string& hs, const std::string& ds, const std::string& gs, const std::string& ps);
	void RemoveGraphicsPipeline(unsigned int idOfGPToRemove);
};


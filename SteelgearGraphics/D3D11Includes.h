#ifndef D3D11_INCLUDES_H
#define D3D11_INCLUDES_H
#pragma once

#define SafeReleaseD3D(x) if (x != NULL) { x->Release(); x = NULL; } // Safe release of D3D pointers

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <string>

struct D3DShaderData
{
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11HullShader* hullShader = nullptr;
	ID3D11DomainShader* domainShader = nullptr;
	ID3D11GeometryShader* geometryShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	std::string identifier;
};

#endif

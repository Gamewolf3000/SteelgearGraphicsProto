#pragma once

enum GraphicsAPI
{
	D3D11
};

struct SGGSettings
{
	struct GraphicsSettings
	{

		GraphicsAPI gAPI = GraphicsAPI::D3D11;
		unsigned int windowWidth = 1280;
		unsigned int windowHeight = 720;

	};

	GraphicsSettings graphics;
};
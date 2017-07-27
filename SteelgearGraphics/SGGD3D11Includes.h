#pragma once

#define SafeReleaseD3D(x) if (x != NULL) { x->Release(); x = NULL; } // Safe release of D3D pointers
#include <d3d11.h>
#include <d3dcompiler.h>
#include <Windows.h>
#pragma comment(lib, "d3d11.lib")
#include "EngineIncludes.h"

void ErrorCheck(FunctionResult result)
{
	try {
		if (result.successLevel == -1)
			throw std::exception();
	}
	catch (std::exception& e) {
		MessageBoxW(0, ((result.errorMessage) + L" Exit code: " + std::to_wstring(result.errorID) + L".").c_str(), L"Function Error", 0);
	}
}

void ErrorCheck(unsigned int errorID, std::wstring errorMessage)
{
	MessageBoxW(0, ((errorMessage) + L" Error code: " + std::to_wstring(errorID) + L".").c_str(), L"Fatal Error", 0);
}

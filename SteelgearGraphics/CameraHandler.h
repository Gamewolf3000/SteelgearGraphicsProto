#ifndef CAMERA_HANDLER_CLASS_H
#define CAMERA_HANDLER_CLASS_H

#include <vector>

#include "SGGEngineIncludes.h"

class SGGEntity;
class TransformHandler;

struct CameraData
{
	Float4x4 viewM;
	Float4x4 projectionM;

	Float3D offset;
	Float3D position;
	Float3D lookAt;
};

class CameraHandler
{
	friend class SGGEntityHandler;

private:
	std::vector<CameraData> cameras;
	std::vector<int> freeSpots;

	CameraData UpdateCameraData(SGGEntity& entity);

public:
	CameraHandler();
	virtual ~CameraHandler();

	void BindCamera(SGGEntity& entity, float fov, float aspectRatio, float nearPlane, float farPlane);
	void RemoveCamera(SGGEntity& entity);

};

#endif
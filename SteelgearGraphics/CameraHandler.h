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

	int transformID;
};

class CameraHandler
{
	friend class SGGEntityHandler;

private:
	std::vector<CameraData> cameras;
	std::vector<int> freeSpots;

	TransformHandler* transformHandler;

	int activeCamera = -1;

	CameraData UpdateActiveCamera();

public:
	CameraHandler(TransformHandler* handler);
	virtual ~CameraHandler();

	void BindCamera(SGGEntity& entity, float fov, float aspectRatio, float nearPlane, float farPlane, bool setActive);
	void RemoveCamera(SGGEntity& entity);
	void SetActiveCamera(SGGEntity& entity);

};

#endif
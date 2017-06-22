#include "CameraHandler.h"
#include "EntityHandler.h"
#include "TransformHandler.h"

CameraData CameraHandler::UpdateActiveCamera()
{
	TransformData data = transformHandler->transforms[cameras[activeCamera].transformID];
	Vec posOffset = VecCreate(cameras[activeCamera].offset.x, cameras[activeCamera].offset.y, cameras[activeCamera].offset.z, 0.0f);
	Vec LookAt = data.position + posOffset + data.direction;

	Matrix tempMV = MatrixViewLH(data.position + posOffset, LookAt, data.up);
	MatrixToFloat4x4(cameras[activeCamera].viewM, tempMV);

	cameras[activeCamera].position = Float3D(VecGetByIndex(0, data.position), VecGetByIndex(1, data.position), VecGetByIndex(2, data.position));
	cameras[activeCamera].lookAt = Float3D(VecGetByIndex(0, LookAt), VecGetByIndex(1, LookAt), VecGetByIndex(2, LookAt));

	return cameras[activeCamera];
}

CameraHandler::CameraHandler(TransformHandler * handler)
{
	transformHandler = handler;
}

CameraHandler::~CameraHandler()
{

}

void CameraHandler::BindCamera(Entity & entity, float fov, float aspectRatio, float nearPlane, float farPlane, bool setActive)
{
	int vectorSpot = -1;

	if (freeSpots.size() > 0)
	{
		vectorSpot = freeSpots[freeSpots.size() - 1];
		cameras[vectorSpot].offset.x = cameras[vectorSpot].offset.y = cameras[vectorSpot].offset.z = 0.0f;

		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
	}
	else
	{
		CameraData temp;
		temp.offset.x = temp.offset.y = temp.offset.z = 0.0f;

		cameras.push_back(temp);
		vectorSpot = cameras.size() - 1;
	}

	if (setActive)
	{
		activeCamera = vectorSpot;
	}

	entity.cameraID = vectorSpot;
	cameras[vectorSpot].transformID = entity.transformID;
	MatrixToFloat4x4(cameras[vectorSpot].projectionM, MatrixProjectionLH(fov, aspectRatio, nearPlane, farPlane));
}

void CameraHandler::RemoveCamera(Entity & entity)
{
	if (entity.cameraID == activeCamera)
	{
		ErrorCheck(5000, L"Active camera removed");
		activeCamera = -1; // This forces an immediate crash, instead of creating a null camera
	}

	freeSpots.push_back(entity.cameraID);
	entity.cameraID = -1;
}

void CameraHandler::SetActiveCamera(Entity & entity)
{
	activeCamera = entity.cameraID;
}

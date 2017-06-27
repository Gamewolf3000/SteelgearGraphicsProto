#include "CameraHandler.h"
#include "SGGEntityHandler.h"
#include "SGGEntity.h"
#include "TransformHandler.h"

CameraData CameraHandler::UpdateCameraData(SGGEntity& entity)
{
	int cameraID = entity.cameraID;
	TransformData* tData = &entity.transform;
	Vec posOffset = VecCreate(cameras[cameraID].offset.x, cameras[cameraID].offset.y, cameras[cameraID].offset.z, 0.0f);
	Vec LookAt = tData->position + posOffset + tData->direction;

	Matrix tempMV = MatrixViewLH(tData->position + posOffset, LookAt, tData->up);
	MatrixToFloat4x4(cameras[cameraID].viewM, tempMV);

	cameras[cameraID].position = Float3D(VecGetByIndex(0, tData->position), VecGetByIndex(1, tData->position), VecGetByIndex(2, tData->position));
	cameras[cameraID].lookAt = Float3D(VecGetByIndex(0, LookAt), VecGetByIndex(1, LookAt), VecGetByIndex(2, LookAt));

	return cameras[cameraID];
}

CameraHandler::CameraHandler()
{
}

CameraHandler::~CameraHandler()
{

}

void CameraHandler::BindCamera(SGGEntity & entity, float fov, float aspectRatio, float nearPlane, float farPlane)
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

	entity.cameraID = vectorSpot;
	MatrixToFloat4x4(cameras[vectorSpot].projectionM, MatrixProjectionLH(fov, aspectRatio, nearPlane, farPlane));
}

void CameraHandler::RemoveCamera(SGGEntity & SGGEntity)
{
	freeSpots.push_back(SGGEntity.cameraID);
	SGGEntity.cameraID = -1;
}

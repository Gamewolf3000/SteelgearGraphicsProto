#include "TransformHandler.h"
#include "EntityHandler.h"

TransformHandler::TransformHandler()
{
}

TransformHandler::~TransformHandler()
{
}

void TransformHandler::BindChild(Entity & parent, Entity & child)
{
	transforms[child.transformID].parent = parent.transformID;
}

void TransformHandler::RemoveParent(Entity & entity)
{
	transforms[entity.transformID].parent = -1;
}

int TransformHandler::CreateTransform()
{
	int returnID = -1;

	if (freeSpots.size() > 0)
	{
		TransformData temp;

		returnID = freeSpots[freeSpots.size() - 1];
		transforms[returnID] = temp;

		freeSpots.pop_back();
		freeSpots.shrink_to_fit();
	}
	else
	{
		TransformData temp;

		transforms.push_back(temp);

		returnID = transforms.size() - 1;
	}

	return returnID;
}

void TransformHandler::RemoveTransform(unsigned int transformID)
{
	transforms[transformID].position = VecCreate(0.0f, 0.0f, 0.0f, 1.0f);
	transforms[transformID].scale = VecCreate(1.0f, 1.0f, 1.0f, 1.0f);
	transforms[transformID].rotation = MatrixIdentity;

	transforms[transformID].direction = VecCreate(0.0f, 0.0f, -1.0f, 0.0f);
	transforms[transformID].right = VecCreate(-1.0f, 0.0f, 0.0f, 0.0f);
	transforms[transformID].up = VecCreate(0.0f, 1.0f, 0.0f, 0.0f);

	transforms[transformID].parent = -1;

	freeSpots.push_back(transformID);
}

Float4x4 TransformHandler::GetEntityTransform(Entity & entity)
{
	Matrix calcMatrix;
	Float4x4 returnMatrix;

	if (transforms[entity.transformID].parent == -1)
	{
		calcMatrix = MatrixScalingFromVector(transforms[entity.transformID].scale) * transforms[entity.transformID].rotation * MatrixTranslationFromVec(transforms[entity.transformID].position);
		MatrixToFloat4x4(returnMatrix, calcMatrix);
	}
	else
	{
		unsigned int parentID = transforms[entity.transformID].parent;
		calcMatrix = MatrixScalingFromVector(transforms[entity.transformID].scale) * transforms[entity.transformID].rotation * MatrixTranslationFromVec(transforms[entity.transformID].position) * MatrixScalingFromVector(transforms[parentID].scale) * transforms[parentID].rotation * MatrixTranslationFromVec(transforms[parentID].position);
		MatrixToFloat4x4(returnMatrix, calcMatrix);
	}

	return returnMatrix;
}

void TransformHandler::SetPosition(Entity & entity, Vec position)
{
	transforms[entity.transformID].position = position;
}

Vec TransformHandler::GetPosition(Entity & entity)
{
	return transforms[entity.transformID].position;
}

void TransformHandler::MoveForward(Entity& entity, float value)
{
	transforms[entity.transformID].position += transforms[entity.transformID].direction * value;
}

void TransformHandler::MoveBackward(Entity & entity, float value)
{
	transforms[entity.transformID].position -= transforms[entity.transformID].direction * value;
}

void TransformHandler::MoveLeft(Entity & entity, float value)
{
	transforms[entity.transformID].position -= transforms[entity.transformID].right * value;
}

void TransformHandler::MoveRight(Entity & entity, float value)
{
	transforms[entity.transformID].position += transforms[entity.transformID].right * value;
}

void TransformHandler::MoveUp(Entity & entity, float value)
{
	transforms[entity.transformID].position += transforms[entity.transformID].up * value;
}

void TransformHandler::MoveDown(Entity & entity, float value)
{
	transforms[entity.transformID].position -= transforms[entity.transformID].up * value;
}

void TransformHandler::MoveAlongVector(Entity & entity, Vec vector)
{
	transforms[entity.transformID].position += vector;
}

Vec TransformHandler::GetForward(Entity& entity)
{
	return transforms[entity.transformID].direction;
}

Vec TransformHandler::GetUp(Entity & entity)
{
	return transforms[entity.transformID].up;
}

Vec TransformHandler::GetRight(Entity & entity)
{
	return transforms[entity.transformID].right;
}

void TransformHandler::Rotate(Entity & entity, const Matrix& rotMatrix)
{
	transforms[entity.transformID].rotation = transforms[entity.transformID].rotation * rotMatrix;

	transforms[entity.transformID].direction = VecMultMatrix3D(VecCreate(0.0f, 0.0f, -1.0f, 0.0f), transforms[entity.transformID].rotation);
	transforms[entity.transformID].right = VecMultMatrix3D(VecCreate(-1.0f, 0.0f, 0.0f, 0.0f), transforms[entity.transformID].rotation);
	transforms[entity.transformID].up = VecMultMatrix3D(VecCreate(0.0f, 1.0f, 0.0f, 0.0f), transforms[entity.transformID].rotation);

	//transforms[entity.transformID].direction = VecCreate(0.0f, 0.0f, -1.0f, 0.0f) * transforms[entity.transformID].rotation;
	//transforms[entity.transformID].right = VecCreate(-1.0f, 0.0f, 0.0f, 0.0f) * transforms[entity.transformID].rotation)
	//transforms[entity.transformID].direction = VecCreate(0.0f, 1.0f, 0.0f, 0.0f) * transforms[entity.transformID].rotation;
}

void TransformHandler::RotatePitch(Entity & entity, float radians)
{
	transforms[entity.transformID].rotation *= MatrixRotationAroundAxis(transforms[entity.transformID].right, radians);

	transforms[entity.transformID].direction = VecMultMatrix3D(VecCreate(0.0f, 0.0f, -1.0f, 0.0f), transforms[entity.transformID].rotation);
	transforms[entity.transformID].up = VecMultMatrix3D(VecCreate(0.0f, 1.0f, 0.0f, 0.0f), transforms[entity.transformID].rotation);
}

void TransformHandler::RotateYaw(Entity & entity, float radians)
{
	transforms[entity.transformID].rotation *= MatrixRotationAroundAxis(transforms[entity.transformID].up, radians);

	transforms[entity.transformID].direction = VecMultMatrix3D(VecCreate(0.0f, 0.0f, -1.0f, 0.0f), transforms[entity.transformID].rotation);
	transforms[entity.transformID].right = VecMultMatrix3D(VecCreate(-1.0f, 0.0f, 0.0f, 0.0f), transforms[entity.transformID].rotation);
}

void TransformHandler::RotateRoll(Entity & entity, float radians)
{
	transforms[entity.transformID].rotation *= MatrixRotationAroundAxis(transforms[entity.transformID].direction, radians);

	transforms[entity.transformID].right = VecMultMatrix3D(VecCreate(-1.0f, 0.0f, 0.0f, 0.0f), transforms[entity.transformID].rotation);
	transforms[entity.transformID].up = VecMultMatrix3D(VecCreate(0.0f, 1.0f, 0.0f, 0.0f), transforms[entity.transformID].rotation);
}

void TransformHandler::SetRotation(Entity & entity, const Matrix& rotMatrix)
{
	transforms[entity.transformID].rotation = rotMatrix;

	transforms[entity.transformID].direction = VecMultMatrix3D(VecCreate(0.0f, 0.0f, -1.0f, 0.0f), transforms[entity.transformID].rotation);
	transforms[entity.transformID].right = VecMultMatrix3D(VecCreate(-1.0f, 0.0f, 0.0f, 0.0f), transforms[entity.transformID].rotation);
	transforms[entity.transformID].up = VecMultMatrix3D(VecCreate(0.0f, 1.0f, 0.0f, 0.0f), transforms[entity.transformID].rotation);
}

Matrix TransformHandler::GetRotation(Entity & entity, const Matrix& rotMatrix)
{
	return transforms[entity.transformID].rotation;
}

void TransformHandler::Scale(Entity & entity, Vec scale)
{
	transforms[entity.transformID].scale = transforms[entity.transformID].scale * scale;
}

void TransformHandler::ScaleUniform(Entity & entity, float value)
{
	transforms[entity.transformID].scale = VecScale(transforms[entity.transformID].scale, value);
}

void TransformHandler::SetScale(Entity & entity, Vec scale)
{
	transforms[entity.transformID].scale = scale;
}

Vec TransformHandler::GetScale(Entity & entity)
{
	return transforms[entity.transformID].scale;
}


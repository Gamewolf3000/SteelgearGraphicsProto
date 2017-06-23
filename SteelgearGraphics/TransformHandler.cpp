#include "TransformHandler.h"
#include "SGGEntityHandler.h"
#include "SGGEntity.h"

TransformHandler::TransformHandler()
{
}

TransformHandler::~TransformHandler()
{
}

void TransformHandler::BindChild(SGGEntity & parent, SGGEntity & child)
{
	child.transform.parent = &parent.transform;
}

void TransformHandler::RemoveParent(SGGEntity & entity)
{
	entity.transform.parent = nullptr;
}

Float4x4 TransformHandler::GetEntityTransform(SGGEntity & entity)
{
	Matrix calcMatrix;
	Float4x4 returnMatrix;

	if (entity.transform.parent == nullptr)
	{
		calcMatrix = MatrixScalingFromVector(entity.transform.scale) * entity.transform.rotation * MatrixTranslationFromVec(entity.transform.position);
		MatrixToFloat4x4(returnMatrix, calcMatrix);
	}
	else
	{
		TransformData parent = *entity.transform.parent;
		calcMatrix = MatrixScalingFromVector(entity.transform.scale) * entity.transform.rotation * MatrixTranslationFromVec(entity.transform.position) * MatrixScalingFromVector(parent.scale) * parent.rotation * MatrixTranslationFromVec(parent.position);
		MatrixToFloat4x4(returnMatrix, calcMatrix);
	}

	return returnMatrix;
}

void TransformHandler::SetPosition(SGGEntity & entity, Vec position)
{
	entity.transform.position = position;
}

Vec TransformHandler::GetPosition(SGGEntity & entity)
{
	return entity.transform.position;
}

void TransformHandler::MoveForward(SGGEntity& entity, float value)
{
	entity.transform.position += entity.transform.direction * value;
}

void TransformHandler::MoveBackward(SGGEntity & entity, float value)
{
	entity.transform.position -= entity.transform.direction * value;
}

void TransformHandler::MoveLeft(SGGEntity & entity, float value)
{
	entity.transform.position -= entity.transform.right * value;
}

void TransformHandler::MoveRight(SGGEntity & entity, float value)
{
	entity.transform.position += entity.transform.right * value;
}

void TransformHandler::MoveUp(SGGEntity & entity, float value)
{
	entity.transform.position += entity.transform.up * value;
}

void TransformHandler::MoveDown(SGGEntity & entity, float value)
{
	entity.transform.position -= entity.transform.up * value;
}

void TransformHandler::MoveAlongVector(SGGEntity & entity, Vec vector)
{
	entity.transform.position += vector;
}

Vec TransformHandler::GetForward(SGGEntity& entity)
{
	return entity.transform.direction;
}

Vec TransformHandler::GetUp(SGGEntity & entity)
{
	return entity.transform.up;
}

Vec TransformHandler::GetRight(SGGEntity & entity)
{
	return entity.transform.right;
}

void TransformHandler::Rotate(SGGEntity & entity, const Matrix& rotMatrix)
{
	entity.transform.rotation = entity.transform.rotation * rotMatrix;

	entity.transform.direction = VecMultMatrix3D(VecCreate(0.0f, 0.0f, -1.0f, 0.0f), entity.transform.rotation);
	entity.transform.right = VecMultMatrix3D(VecCreate(-1.0f, 0.0f, 0.0f, 0.0f), entity.transform.rotation);
	entity.transform.up = VecMultMatrix3D(VecCreate(0.0f, 1.0f, 0.0f, 0.0f), entity.transform.rotation);

	//transforms[SGGEntity.transformID].direction = VecCreate(0.0f, 0.0f, -1.0f, 0.0f) * transforms[SGGEntity.transformID].rotation;
	//transforms[SGGEntity.transformID].right = VecCreate(-1.0f, 0.0f, 0.0f, 0.0f) * transforms[SGGEntity.transformID].rotation)
	//transforms[SGGEntity.transformID].direction = VecCreate(0.0f, 1.0f, 0.0f, 0.0f) * transforms[SGGEntity.transformID].rotation;
}

void TransformHandler::RotatePitch(SGGEntity & entity, float radians)
{
	entity.transform.rotation *= MatrixRotationAroundAxis(entity.transform.right, radians);

	entity.transform.direction = VecMultMatrix3D(VecCreate(0.0f, 0.0f, -1.0f, 0.0f), entity.transform.rotation);
	entity.transform.up = VecMultMatrix3D(VecCreate(0.0f, 1.0f, 0.0f, 0.0f), entity.transform.rotation);
}

void TransformHandler::RotateYaw(SGGEntity & entity, float radians)
{
	entity.transform.rotation *= MatrixRotationAroundAxis(entity.transform.up, radians);

	entity.transform.direction = VecMultMatrix3D(VecCreate(0.0f, 0.0f, -1.0f, 0.0f), entity.transform.rotation);
	entity.transform.right = VecMultMatrix3D(VecCreate(-1.0f, 0.0f, 0.0f, 0.0f), entity.transform.rotation);
}

void TransformHandler::RotateRoll(SGGEntity & entity, float radians)
{
	entity.transform.rotation *= MatrixRotationAroundAxis(entity.transform.direction, radians);

	entity.transform.right = VecMultMatrix3D(VecCreate(-1.0f, 0.0f, 0.0f, 0.0f), entity.transform.rotation);
	entity.transform.up = VecMultMatrix3D(VecCreate(0.0f, 1.0f, 0.0f, 0.0f), entity.transform.rotation);
}

void TransformHandler::SetRotation(SGGEntity & entity, const Matrix& rotMatrix)
{
	entity.transform.rotation = rotMatrix;

	entity.transform.direction = VecMultMatrix3D(VecCreate(0.0f, 0.0f, -1.0f, 0.0f), entity.transform.rotation);
	entity.transform.right = VecMultMatrix3D(VecCreate(-1.0f, 0.0f, 0.0f, 0.0f), entity.transform.rotation);
	entity.transform.up = VecMultMatrix3D(VecCreate(0.0f, 1.0f, 0.0f, 0.0f), entity.transform.rotation);
}

Matrix TransformHandler::GetRotation(SGGEntity & entity, const Matrix& rotMatrix)
{
	return entity.transform.rotation;
}

void TransformHandler::Scale(SGGEntity & entity, Vec scale)
{
	entity.transform.scale = entity.transform.scale * scale;
}

void TransformHandler::ScaleUniform(SGGEntity & entity, float value)
{
	entity.transform.scale = VecScale(entity.transform.scale, value);
}

void TransformHandler::SetScale(SGGEntity & entity, Vec scale)
{
	entity.transform.scale = scale;
}

Vec TransformHandler::GetScale(SGGEntity & entity)
{
	return entity.transform.scale;
}


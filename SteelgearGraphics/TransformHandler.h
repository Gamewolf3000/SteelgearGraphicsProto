#ifndef TRANSFORM_HANDLER_H
#define TRANSFORM_HANDLER_H
#pragma once

#include "EngineIncludes.h"

struct Entity;

class TransformHandler
{
	friend class EntityHandler;
	friend class CameraHandler;

private:
	std::vector<TransformData> transforms;
	std::vector<int> freeSpots;

	int CreateTransform();
	void RemoveTransform(unsigned int transformID);

	Float4x4 GetEntityTransform(Entity& entity);

public:

	TransformHandler();
	virtual ~TransformHandler();

	void BindChild(Entity& parent, Entity& child);
	void RemoveParent(Entity& entity);

	void SetPosition(Entity& entity, Vec position);
	Vec GetPosition(Entity& entity);

	void MoveForward(Entity& entity, float value);
	void MoveBackward(Entity& entity, float value);
	void MoveLeft(Entity& entity, float value);
	void MoveRight(Entity& entity, float value);
	void MoveUp(Entity& entity, float value);
	void MoveDown(Entity& entity, float value);
	void MoveAlongVector(Entity& entity, Vec vector);

	Vec GetForward(Entity& entity);
	Vec GetUp(Entity& entity);
	Vec GetRight(Entity& entity);

	void Rotate(Entity& entity, const Matrix& rotMatrix);
	void RotatePitch(Entity& entity, float radians);
	void RotateYaw(Entity& entity, float radians);
	void RotateRoll(Entity& entity, float radians);
	void SetRotation(Entity& entity, const Matrix& rotMatrix);
	Matrix GetRotation(Entity& entity, const Matrix& rotMatrix);

	void Scale(Entity& entity, Vec scale);
	void ScaleUniform(Entity& entity, float value);
	void SetScale(Entity& entity, Vec scale);
	Vec GetScale(Entity& entity);


};

#endif

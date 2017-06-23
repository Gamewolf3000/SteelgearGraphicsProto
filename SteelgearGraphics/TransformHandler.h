#ifndef TRANSFORM_HANDLER_H
#define TRANSFORM_HANDLER_H
#pragma once

#include <vector>

#include "SGGEngineIncludes.h"
#include "SGGMath.h"

class SGGEntity;

class TransformHandler
{
	friend class SGGEntityHandler;
	friend class CameraHandler;

private:
	//std::vector<TransformData> transforms;
	//std::vector<int> freeSpots;

	//int CreateTransform();
	//void RemoveTransform(unsigned int transformID);

	Float4x4 GetEntityTransform(SGGEntity& entity);

public:
	TransformHandler();
	virtual ~TransformHandler();

	void BindChild(SGGEntity& parent, SGGEntity& child);
	void RemoveParent(SGGEntity& entity);

	void SetPosition(SGGEntity& entity, Vec position);
	Vec GetPosition(SGGEntity& entity);

	void MoveForward(SGGEntity& entity, float value);
	void MoveBackward(SGGEntity& entity, float value);
	void MoveLeft(SGGEntity& entity, float value);
	void MoveRight(SGGEntity& entity, float value);
	void MoveUp(SGGEntity& entity, float value);
	void MoveDown(SGGEntity& entity, float value);
	void MoveAlongVector(SGGEntity& entity, Vec vector);

	Vec GetForward(SGGEntity& entity);
	Vec GetUp(SGGEntity& entity);
	Vec GetRight(SGGEntity& entity);

	void Rotate(SGGEntity& entity, const Matrix& rotMatrix);
	void RotatePitch(SGGEntity& entity, float radians);
	void RotateYaw(SGGEntity& entity, float radians);
	void RotateRoll(SGGEntity& entity, float radians);
	void SetRotation(SGGEntity& entity, const Matrix& rotMatrix);
	Matrix GetRotation(SGGEntity& entity, const Matrix& rotMatrix);

	void Scale(SGGEntity& entity, Vec scale);
	void ScaleUniform(SGGEntity& entity, float value);
	void SetScale(SGGEntity& entity, Vec scale);
	Vec GetScale(SGGEntity& entity);


};

#endif

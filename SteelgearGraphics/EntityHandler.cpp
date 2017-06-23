//#include "EntityHandler.h"
//
//std::vector<MeshData>& EntityHandler::GetMeshJobs()
//{
//	return meshHandler->GetMeshJobs();
//}
//
//void EntityHandler::ClearMeshJobs()
//{
//	meshHandler->ClearJobVector();
//}
//
//std::vector<ShaderData>& EntityHandler::GetShaderJobs()
//{
//	return shaderHandler->GetShaderJobs();
//}
//
//void EntityHandler::ClearShaderJobs()
//{
//	shaderHandler->ClearShaderJobs();
//}
//
//std::vector<MaterialData>& EntityHandler::GetMaterialJobs()
//{
//	return materialHandler->GetMaterialJobs();
//}
//
//void EntityHandler::ClearMaterialJobs()
//{
//	materialHandler->ClearJobVector();
//}
//
//std::vector<EnvironmentMapJob>& EntityHandler::GetEnvironmentMapJobs()
//{
//	return environmentMapJobs;
//}
//
//void EntityHandler::ClearEnvironmentMapJobs()
//{
//	environmentMapJobs.clear();
//}
//
//Float4x4 EntityHandler::GetEntityTransform(Entity & entity)
//{
//	return transformHandler->GetEntityTransform(entity);
//}
//
//LightData EntityHandler::GetLightData(Entity & entity)
//{
//	return lightHandler->lights[entity.lightID];
//}
//
//std::vector<Entity*>& EntityHandler::GetEntities()
//{
//	return entities;
//}
//
//CameraData EntityHandler::GetActiveCamera()
//{
//	return cameraHandler->UpdateActiveCamera();
//}
//
//EntityHandler::EntityHandler(wchar_t directory[200])
//{
//	transformHandler = new TransformHandler;
//	meshHandler = new MeshHandler(directory);
//	shaderHandler = new ShaderHandler(directory);
//	cameraHandler = new CameraHandler(transformHandler);
//	materialHandler = new MaterialHandler;
//	lightHandler = new LightHandler;
//}
//
//EntityHandler::~EntityHandler()
//{
//	for (int i = 0; i < entities.size(); i++)
//	{
//		delete entities[i];
//	}
//
//	delete transformHandler;
//	delete meshHandler;
//	delete shaderHandler;
//	delete cameraHandler;
//	delete materialHandler;
//	delete lightHandler;
//}
//
//Entity * EntityHandler::CreateEntity()
//{
//	Entity* temp = new Entity;
//
//	if (freeSpots.size() > 0)
//	{
//		entities[freeSpots[freeSpots.size() - 1]] = temp;
//		temp->idNumber = freeSpots[freeSpots.size() - 1];
//
//		freeSpots.pop_back();
//		freeSpots.shrink_to_fit();
//	}
//	else
//	{
//		entities.push_back(temp);
//		temp->idNumber = entities.size() - 1;
//	}
//
//	temp->transformID = transformHandler->CreateTransform();
//
//	return temp;
//}
//
//Entity * EntityHandler::CreateEntity(std::string identifier)
//{
//	Entity* temp = new Entity;
//
//	if (freeSpots.size() > 0)
//	{
//		entities[freeSpots[freeSpots.size() - 1]] = temp;
//		temp->idNumber = freeSpots[freeSpots.size() - 1];
//
//		freeSpots.pop_back();
//		freeSpots.shrink_to_fit();
//	}
//	else
//	{
//		entities.push_back(temp);
//		temp->idNumber = entities.size() - 1;
//	}
//
//	temp->transformID = transformHandler->CreateTransform();
//	temp->identifier = identifier;
//
//	return temp;
//}
//
//void EntityHandler::DeleteEntity(Entity & entity)
//{
//	transformHandler->RemoveTransform(entity.transformID);
//
//	if (entity.cameraID != -1)
//	{
//		cameraHandler->RemoveCamera(entity);
//	}
//
//	if (entity.meshID != -1)
//	{
//		meshHandler->RemoveMesh(entity);
//	}
//
//	if (entity.shaderID != -1)
//	{
//		shaderHandler->RemoveShaders(entity);
//	}
//
//	materialHandler->RemoveMaterials(entity, diffuse);
//	materialHandler->RemoveMaterials(entity, roughness);
//	materialHandler->RemoveMaterials(entity, metallic);
//	materialHandler->RemoveMaterials(entity, normal);
//	materialHandler->RemoveMaterials(entity, displacement);
//
//	unsigned int idNr = entity.idNumber;
//	delete entities[idNr];
//	entities[idNr] = nullptr;
//	entities.erase(entities.begin() + idNr);
//	entities.shrink_to_fit();
//
//}
//
//TransformHandler* EntityHandler::Transform()
//{
//	return transformHandler;
//}
//
//MeshHandler * EntityHandler::Mesh()
//{
//	return meshHandler;
//}
//
//ShaderHandler * EntityHandler::Shader()
//{
//	return shaderHandler;
//}
//
//CameraHandler * EntityHandler::Camera()
//{
//	return cameraHandler;
//}
//
//MaterialHandler * EntityHandler::Material()
//{
//	return materialHandler;
//}
//
//LightHandler * EntityHandler::Light()
//{
//	return lightHandler;
//}
//
//void EntityHandler::SetEnvironmentMap(Entity& entity, EnvironmentMap typeOfMap, int updateDelay)
//{
//	EnvironmentMapJob temp;
//	temp.typeOfMap = typeOfMap;
//	temp.entityID = entity.idNumber;
//	environmentMapJobs.push_back(temp);
//
//	entity.environmentMapInfo.typeOfMap = typeOfMap;
//	entity.environmentMapInfo.updateDelay = updateDelay;
//	entity.environmentMapInfo.needsUpdate = true;
//}
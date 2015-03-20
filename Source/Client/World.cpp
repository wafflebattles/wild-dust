#include "Precompiled.h"
#include "World.h"
#include "Entity.h"
#include "Shared/Protocol.h"

ClientWorld::ClientWorld(Client *client) {
	mClient = client;
}

ClientWorld::~ClientWorld() {
	deleteAllEntities();
}

void ClientWorld::registerEntityType(const std::string &typeName, CreateClientEntityFunc func) {
	auto result = mCreateFuncs.find(typeName);
	if (result == mCreateFuncs.end()) {
		mCreateFuncs.insert({ typeName, func });
	}
}

CL_Entity *ClientWorld::findEntityByID(EntityID id) {
	auto result = mIDToEntityMap.find(id);
	if (result != mIDToEntityMap.end()) {
		return result->second;
	}
	return NULL;
}

EntityID ClientWorld::findIDByEntity(CL_Entity *entity) {
	auto result = mEntityToIDMap.find(entity);
	if (result != mEntityToIDMap.end()) {
		return result->second;
	}
	return 0;
}

void ClientWorld::deleteAllEntities() {
	for (CL_Entity *entity : mEntities) {
		delete entity;
	}
	mEntities.clear();
}

void ClientWorld::readFromSnapshot(const BitStream &snapshot) {
	uint32_t numEnts = snapshot.readU32();
	for (uint32_t i = 0; i < numEnts; ++i) {
		EntityID entityID = snapshot.readAny<EntityID>();
		std::string entityTypeName = snapshot.readString();

		CL_Entity *entity = findEntityByID(entityID);
		if (entity == NULL) {
			auto createFunc = mCreateFuncs.find(entityTypeName);
			if (createFunc != mCreateFuncs.end()) {
				entity = createFunc->second(mClient);
				entity->mTypeName = entityTypeName;
				addEntity(entityID, entity);
			}
		}

		entity->readFromStream(snapshot);
	}

	uint32_t numRemovedEnts = snapshot.readU32();
	for (uint32_t i = 0; i < numRemovedEnts; ++i) {
		EntityID id = snapshot.readAny<EntityID>();
	}
}

void ClientWorld::addEntity(EntityID id, CL_Entity *entity) {
	entity->mID = id;

	mIDToEntityMap.insert({ id, entity });
	mEntityToIDMap.insert({ entity, id });

	mEntities.push_back(entity);
}

void ClientWorld::update(float dt) {
	for (CL_Entity *entity : mEntities) {
		entity->update(dt);
	}
}

void ClientWorld::draw() {
	for (CL_Entity *entity : mEntities) {
		entity->draw();
	}
}
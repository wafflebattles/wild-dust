#include "Precompiled.h"
#include "ProjectileController.h"
#include "Core/Entity.h"
#include <glm/gtx/norm.hpp>
#include "GameContext.h"
#include "Core/World.h"
#include "Core/Collision.h"
#include "GibSpawner.h"

// TODO Put projectile stats somewhere else

ProjectileController::ProjectileController(GameContext *context) : mContext(context), mPositionSet(false) {

}

void ProjectileController::control(Entity *e, float gameTime, float dt) {
	auto velocity = e->getVelocity();
	auto position = e->getPosition();

	// Unfortunately doing in onAttached might not be actual position if position is changed immediately after creating projectile
	if (!mPositionSet) {
		mPositionSet = true;
		mInitialPosition = position;
	}

	Vec3 nextPosition = position + velocity * dt;
	auto &entities = mContext->getWorld()->getEntities();

	float nearestTime = 1000.0f;
	Entity *collider = nullptr;
	for (auto &kv : entities) {
		auto other = kv.second;
		const auto &type = other->getType();
		if (other != e && other->getOwner() != e->getOwner() && type != "bullet" && type != "gibs") {
			float time = Collision::collidesBullet(e, other, position, nextPosition);
			if (time >= 0 && time <= 1.0f && (time < nearestTime || !collider)) {
				nearestTime = time;
				collider = other;
			}
		}
	}

	if (collider) {
		position = collider->getCenteredPosition();
		mContext->getWorld()->scheduleDeleteEntity(e);
		auto gibSpawner = mContext->getGibSpawner();
		if (gibSpawner)
			gibSpawner->spawnGibs(position, 15);
	}
	else {
		position = nextPosition;
	}

	e->setPosition(position);

	auto world = mContext ? mContext->getWorld() : nullptr;
	if (glm::distance2(position, mInitialPosition) > 500 * 500 && world) {
		world->scheduleDeleteEntity(e);
	}
}

void ProjectileController::onAttached(Entity *e) {
	if (e) mInitialPosition = e->getCenteredPosition();
}

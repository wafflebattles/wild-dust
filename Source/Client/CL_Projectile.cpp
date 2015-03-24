#include "Precompiled.h"
#include "CL_Projectile.h"
#include "Texture.h"
#include "Client.h"
#include "Renderer.h"
#include "SpriteBatcher.h"
#include "Physics/PhysicsObject.h"

CL_Projectile::CL_Projectile(Client *client) : CL_PhysicsEntity(client, false) {
	mTexture = client->getRenderer()->getTexture("../Content/Textures/Misc/Bullet.png");
}

CL_Projectile::~CL_Projectile() {
	mTexture->decrementRefs();
}

void CL_Projectile::readFromStream(const BitStream &stream) {
	CL_PhysicsEntity::readFromStream(stream);
}

void CL_Projectile::draw() {
	Recti source;
	source.x = 0;
	source.y = 0;
	source.w = mTexture->getWidth();
	source.h = mTexture->getHeight();
	Renderer *renderer = mClient->getRenderer();
	SpriteBatcher *batcher = renderer->getSpriteBatcher(mTexture, BLEND_ALPHA);
	batcher->addSprite(getPhysicsObject()->getPosition(), Vec2(8, 8), source, Color(1.0f), FLIP_NONE);
}
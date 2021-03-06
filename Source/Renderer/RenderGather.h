#pragma once

#include "BlendMode.h"
#include "GUI.h"
#include <vector>

class World;
class Renderer;
class Entity;

// This gathers entities from World and prepares it for Renderer since not the responsibility of the renderer to
// gather
class RENDERER_API RenderGather {
public:
	void drawWorld(World *world, Renderer *renderer, GUI *gui, const std::vector<GUIData> &data);

private:
	void drawStars(Entity *ent, Renderer *renderer);
	void drawSprite(Entity *ent, Renderer *renderer);
	void drawTilemap(Entity *ent, Renderer *renderer);
	void drawGibs(Entity *ent, Renderer *renderer);
};

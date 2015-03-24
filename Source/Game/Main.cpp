#include "Core/Precompiled.h"
#include "Core/Core.h"
#include "Core/Config.h"
#include <enet/enet.h>

#include "Core/EventEmitter.h"
#include "Network/Connection.h"

#include "Renderer/Precompiled.h"
#include "Renderer/SDLContext.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderGather.h"

#include "Client/ClientWorld.h"
#include "Server/ServerWorld.h"

#include "Input/Input.h"
#include "Core/EntityFactory.h"
#include "Core/Entity.h"

#ifndef _WIN32
bool ArgvContains(char *argv[], int argc, const char *what) {
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], what) == 0) {
			return true;
		}
	}
	return false;
}
#endif

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

	gCore.init();

	Config settings;
	settings.loadFromFile("Config/Game.cfg");

	SDLContext context;
	context.init(settings);

	RenderGather renderGather;
	Renderer renderer(&context);
	renderer.init();

	InputSystem input;

	ClientWorld clientWorld(&input, nullptr);
	ServerWorld *serverWorld = nullptr;

#ifdef _WIN32
	if (strstr(lpCmdLine, "-server")) {
#else
	if (ArgvContains(argv, argc, "-server")) {
#endif
		serverWorld = new ServerWorld();
		serverWorld->serve(1336);
	}
	clientWorld.connect("127.0.0.1", 1336);

	// auto ent = clientWorld.addEntity("name", EntityFactory::createEntity("no", "ones"));
	// ent->setPosition(Vec3(20, 20, 0));

	Clock clock;
	while (!context.isQuitSignaled()) {
		context.tick();

		float dt = clock.getElapsedSeconds();
		clock.reset();

		clientWorld.update(dt);
		if (serverWorld)
			serverWorld->update(dt);
		input.update(dt);

		renderGather.drawWorld(&clientWorld, &renderer);
	}
		
	delete serverWorld;
	gCore.shutdown();

	enet_deinitialize();

	return 0;
}
#pragma once

#include "Core/Clock.h"
#include "Core/BitStream.h"

class Renderer;
class ClientWorld;
class InputSystem;
class AudioSystem;

enum ClientNetworkState {
	CLIENT_IDLE,
	CLIENT_CONNECTING,
	CLIENT_CONNECTED,
};

class CLIENT_API Client {
public:
	Client();
	~Client();

	bool init();
	void shutdown();

	void connectToServer(const char *hostName, int serverPort);
	void disconnectFromServer();

	void tick();

	SDL_Window *getGameWindow() const { return mGameWindow; }
	int getWindowWidth() const { return mWindowWidth; }
	int getWindowHeight() const { return mWindowHeight; }
	bool isQuitSignaled() const { return mQuitSignaled; }

	Renderer *getRenderer() const { return mRenderer; }

private:
	void sendPlayerInput();
	void processNetworkEvents();
	void handleConnectEvent();
	void handleDisconnectEvent();
	void handleReceiveEvent(const BitStream &stream);

	void renderFrame();

private:
	SDL_Window *mGameWindow;
	int mWindowWidth;
	int mWindowHeight;
	bool mQuitSignaled;
	Clock mTickTock;
	Renderer *mRenderer;
	ClientWorld *mWorld;
	InputSystem *mInput;
	AudioSystem *mAudio;

private:
	ENetHost *mHost;
	ENetPeer *mPeer;
	ClientNetworkState mNetworkState;
	Clock mConnectionClock;
	Clock mSendInputClock;
};

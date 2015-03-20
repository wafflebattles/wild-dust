#pragma once

class Client;
class Shader;
class Texture;

class Renderer {
public:
	Renderer(Client *context);

	bool init();
	void shutdown();

	void beginFrame();
	void endFrame();

private:
	Client *mContext;
	SDL_GLContext mGLContext;
	glm::mat4 mProjMatrix;
	glm::mat4 mViewMatrix;
	Shader *m2DShader;
	Texture *mGrass;
	GLuint mVAO;
	GLuint mVBO;
	GLuint mIBO;
};

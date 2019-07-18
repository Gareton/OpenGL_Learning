#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H
#include <glad/glad.h>
#include "Shader.h"
#include "Texture.h"

class PostProcessor
{
public:

	Shader PostProcessingShader;
	Texture2D Texture;
	GLuint Width, Height;

	GLboolean Confuse, Chaos, Shake;

	PostProcessor(Shader shader, GLuint width, GLuint height);

	void BeginRender();

	void EndRender();

	void Render(GLfloat time);
private:

	GLuint MSFBO, FBO; 
	GLuint RBO; 
	GLuint VAO;

	void initRenderData();
};

#endif

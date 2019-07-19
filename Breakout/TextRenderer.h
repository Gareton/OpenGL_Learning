#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include "Shader.h"


struct Character {
	GLuint TextureID;   
	glm::ivec2 Size;    
	glm::ivec2 Bearing; 
	GLuint Advance;     
};

class TextRenderer
{
public:
	TextRenderer(GLuint width, GLuint height);
	void Load(std::string font, GLuint fontSize);
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color = glm::vec4(1.0f));
private:
	GLuint VAO, VBO;
	std::map<GLchar, Character> Characters;
	Shader TextShader;
};

#endif

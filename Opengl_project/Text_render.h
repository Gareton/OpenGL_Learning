#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H 
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <map>
#include <iostream>
#include "Shader.h"

namespace tsp {

	struct Character {
		GLuint textureID;
		glm::ivec2 size;
		glm::ivec2 bearing;
		GLuint advance;
	};

	std::map<GLchar, Character> characters;
	glm::mat4 projection;
	GLuint VAO, VBO;
	

	void init(const GLchar *font_path, GLuint height, float scr_width, float scr_height)
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

		FT_Face face;
		if (FT_New_Face(ft, font_path, 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		FT_Set_Pixel_Sizes(face, 0, height);

		for (GLubyte c = 0; c < 128; ++c)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};

			characters.insert(std::pair<GLchar, Character>(c, character));
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		projection = glm::ortho(0.0f, scr_height, 0.0f, scr_width);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDisable(GL_BLEND);
	}

	void render_text(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		s.use();
		glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
		s.setUniformMat4("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		std::string::const_iterator c;

		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = characters[*c];

			GLfloat xpos = x + ch.bearing.x * scale;
			GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

			GLfloat w = ch.size.x * scale;
			GLfloat h = ch.size.y * scale;

			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};

			glBindTexture(GL_TEXTURE_2D, ch.textureID);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (ch.advance >> 6) * scale; 
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}

#endif

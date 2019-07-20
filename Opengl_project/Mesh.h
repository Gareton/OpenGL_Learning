#ifndef MESH_H
#define MESH_H
#include "Structures.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include "Shader.h"

namespace mdl {

	class Mesh {
	public:
		Mesh(std::vector<struc::Vertex> vertices, std::vector<unsigned int> indices, std::vector<struc::Texture> textures);
		void draw(Shader s);
	private:
		void setupMesh();

		std::vector<struc::Vertex> _vertices;
		std::vector<unsigned int> _indices;
		std::vector<struc::Texture> _textures;

		unsigned int _VAO, _VBO, _EBO;
	};

	Mesh::Mesh(std::vector<struc::Vertex> vertices, std::vector<unsigned int> indices, std::vector<struc::Texture> textures)
		: _vertices(vertices), _indices(indices), _textures(textures)
	{
		setupMesh();
	}

	void Mesh::setupMesh()
	{
		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);
		glGenBuffers(1, &_EBO);

		glBindVertexArray(_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(struc::Vertex), &_vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struc::Vertex), (void*)(0));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struc::Vertex), (void*)(offsetof(struc::Vertex, normal)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struc::Vertex), (void*)(offsetof(struc::Vertex, texCords)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	void Mesh::draw(Shader s)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;

		for (int i = 0; i < _textures.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string nr;

			if (_textures[i].type == "texture_diffuse")
			{
				nr = std::to_string(diffuseNr);
				++diffuseNr;
			}
			else if (_textures[i].type == "texture_specular")
			{
				nr = std::to_string(specularNr);
				++specularNr;
			}

			s.setUniformValue(("material." + _textures[i].type + nr).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, _textures[i].id);


		}

		glBindVertexArray(_VAO);
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}
}

#endif
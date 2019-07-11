#ifndef MODEL_H
#define MODEL_H
#include "Mesh.h"
#include "texture_loader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <set>

namespace mdl {


	class Model {
	public:
		Model(const GLchar *path)
		{
			loadModel(path);
		}
		void draw(Shader s);
	private:
		void loadModel(std::string path);
		void processNode(aiNode *node);
		Mesh processMesh(aiMesh *mesh);
		std::vector<struc::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
		unsigned int TextureFromFile(std::string path);
		std::string createCorrectPath(std::string path);

		std::vector<Mesh> _meshes;
		std::string _directory;
		const aiScene *_scene;
		std::set<struc::Texture, struc::TexCompare> _loaded_textures;
	};


	void Model::draw(Shader s)
	{
		for (int i = 0; i < _meshes.size(); ++i)
			_meshes[i].draw(s);
	}

	void Model::loadModel(std::string path)
	{
		Assimp::Importer importer;
		_scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

		if (!_scene || _scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP:" << importer.GetErrorString() << std::endl;
		}

		_directory = path.substr(0, path.find_last_of('\\') + 1);

		processNode(_scene->mRootNode);
	}

	void Model::processNode(aiNode *node)
	{
		for (size_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh *mesh = _scene->mMeshes[node->mMeshes[i]];
			_meshes.push_back(processMesh(mesh));
		}

		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i]);
		}
	}

	Mesh Model::processMesh(aiMesh *mesh)
	{
		std::vector<struc::Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<struc::Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			struc::Vertex v;

			v.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			v.texCords = glm::vec2();

			if (mesh->mTextureCoords[0]) 
			{
				v.texCords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else
				v.texCords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(v);
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			
			for (size_t j = 0; j < face.mNumIndices; ++j)
				indices.push_back(face.mIndices[j]);
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = _scene->mMaterials[mesh->mMaterialIndex];

			std::vector<struc::Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			std::vector<struc::Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<struc::Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<struc::Texture> textures;

		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{

			aiString another_path;
			mat->GetTexture(type, i, &another_path);

			std::string correct_path = createCorrectPath(another_path.C_Str());

			auto it = _loaded_textures.find(struc::Texture(0, std::string(), correct_path));

			if (it == _loaded_textures.end())
			{
				struc::Texture texture;

				texture.id = TextureFromFile(correct_path);
				texture.type = typeName;
				texture.path = correct_path;
				textures.push_back(texture);
				_loaded_textures.insert(texture);
			}
			else
			{
				textures.push_back(*it);
			}
		}

		return textures;
	}

	

	unsigned int Model::TextureFromFile(std::string path)
	{
		Texture tex(path.c_str());

		return tex.ID;
	}

	std::string Model::createCorrectPath(std::string path)
	{
		std::string texName = path.substr(path.find_last_of('\\') + 1, path.size() - (path.find_last_of('\\') + 1));

		return _directory + texName;
	}

}
#endif

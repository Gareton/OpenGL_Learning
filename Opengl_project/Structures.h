#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace struc {

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCords;
	};

	struct Texture {
		Texture(unsigned int id = 0, std::string type = "", std::string path = "")
		{
			this->id = id;
			this->type = type;
			this->path = path;
		}

		unsigned int id;
		std::string type;
		std::string path;

	};

	class TexCompare {
	public:
		bool operator()(Texture t1, Texture t2) const { return t1.path < t2.path; }
	};

}

#endif

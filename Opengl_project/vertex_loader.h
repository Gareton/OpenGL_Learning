#ifndef VERTEX_LOADER_H
#define VERTEX_LOADER_H
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <vector>

void load_verticies(const GLchar *path, std::vector<float> &buf)
{
	std::ifstream in;

	in.open(path, std::ios::in | std::ios::binary);

	if (!in.is_open())
	{
		std::cout << "Cannot open the file: " << path << std::endl;
		return;
	}

	for (int i = 0; !in.eof(); ++i)
	{
		float t = 0.0f;
		in >> t;
		buf.push_back(t);
	}

	in.close();
}

#endif

#pragma once
#include <vector>
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "CubeMap.h"

struct Material {

	Material(const char* matFile);
	void Bind();

	Shader shader;
	std::vector<CubeMap> cubeMaps;
	std::vector<Texture> textures;

};

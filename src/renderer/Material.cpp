#include <array>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include "Material.h"

Material::Material(const char* matFile) {

	std::ifstream file(matFile);
	std::stringstream sstream;

	sstream << file.rdbuf();
	file.close();

	std::string line;
	int textureUniformId = 0;

	while (std::getline(sstream, line)) {
		size_t firstColon = line.find(":");
		size_t secondColon = line.find(":", firstColon + 1);

		std::string type = line.substr(0, firstColon);
		std::string name = line.substr(firstColon + 1, secondColon - (firstColon + 1));
		std::string value = line.substr(secondColon + 1, line.size() - secondColon);

		if (type == "shader") {
			std::string vert = value + ".vert";
			std::string frag = value + ".frag";
			shader = Shader(vert, frag);
			continue;
		}

		if (type == "cubemap") {
			std::array<std::string, 6> cubeMapTextures;

			for (int i = 0; i < 6; i++) {
				std::getline(sstream, line);
				cubeMapTextures[i] = line;
			}

			cubeMaps.push_back(CubeMap(cubeMapTextures));
			shader.SetInt(name.c_str(), textureUniformId);
			textureUniformId++;

			continue;
		}

		if (type == "texture") {
			textures.push_back(Texture(value.c_str()));
			shader.SetInt(name.c_str(), textureUniformId);
			textureUniformId++;
			continue;
		}

		if (type == "float") {
			float f = std::atof(value.c_str());
			shader.SetFloat(name.c_str(), f);
			continue;
		}

	}

}

void Material::Bind() {
	shader.Bind();

	int textureUnit = 0;

	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		textures[i].Bind();
		textureUnit++;
	}

	for (int i = 0; i < cubeMaps.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		cubeMaps[i].Bind();
		textureUnit++;
	}
}

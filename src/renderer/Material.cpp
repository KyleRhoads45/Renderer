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
		size_t pivot = line.find(":");
		std::string type = line.substr(0, pivot);
		std::string value = line.substr(pivot + 1, line.size() - pivot);

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
			glUniform1i(glGetUniformLocation(shader.shaderId, value.c_str()), 0);
			continue;
		}

		textures.push_back(Texture(value.c_str()));

		glUniform1iv(glGetUniformLocation(shader.shaderId, type.c_str()), 1, &textureUniformId);
		textureUniformId++;
	}

}

void Material::Bind() {
	shader.Bind();

	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		textures[i].Bind();
	}
	for (int i = 0; i < cubeMaps.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		cubeMaps[i].Bind();
	}
}

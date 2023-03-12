#include <cassert>
#include <stb/stb_image.h>
#include <glad/glad.h>
#include "Texture.h"

Texture::Texture(const char* filePath) {
    stbi_set_flip_vertically_on_load(true);

    int numChannels;
    unsigned char* imageData = stbi_load(filePath, &width, &height, &numChannels, 0);

    assert(imageData);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    if (numChannels == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_R16, GL_UNSIGNED_BYTE, imageData);
    }
    else if (numChannels == 2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, imageData);
    }
    else if (numChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    }

    stbi_image_free(imageData);
    stbi_set_flip_vertically_on_load(false);
}

void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}

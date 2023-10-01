﻿#include "Texture.h"

#include <stdexcept>
#include <string>

Texture::Texture() :
	textureID(0),
	width(0),
	height(0),
	bitDepth(0),
	fileLocation("")
{}

Texture::Texture(const char* fileLoc) : Texture()
{
	fileLocation = fileLoc;
}
void Texture::LoadTexture()
{
	stbi_uc *texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);
	if(!texData)
	{
		throw std::runtime_error("Failed to find: " + std::string(fileLocation) + "\n");
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);
}

void Texture::ClearTexture()
{
	glDeleteTextures(1, &textureID);
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = nullptr;
}


void Texture::UseTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

Texture::~Texture()
{
	ClearTexture();
}

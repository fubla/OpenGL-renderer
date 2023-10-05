#include "ShadowMap.h"

#include <string>

ShadowMap::ShadowMap() :
	FBO(0),
	shadowWidth(0)
{}

bool ShadowMap::Init(unsigned int width, unsigned int height)
{
	shadowWidth = width;
	shadowHeight = height;

	glGenFramebuffers(1, &FBO);

	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	constexpr float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

	// Ignore color attachments
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Error: %i\n", status);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void ShadowMap::Write()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

}

void ShadowMap::Read(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
}

GLuint ShadowMap::GetShadowWidth()
{
	return shadowWidth;
}

GLuint ShadowMap::GetShadowHeight()
{
	return shadowHeight;
}

ShadowMap::~ShadowMap()
{
	if(FBO)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	if(shadowMap)
	{
		glDeleteTextures(1, &shadowMap);
	}
}

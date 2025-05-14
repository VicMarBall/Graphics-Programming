#include "bloom.h"

void BloomResources::Init(const int& screenWidth, const int& screenHeight)
{
	// bloom mipmap
	if (rtBright != 0) { glDeleteTextures(1, &rtBright); }
	glGenTextures(1, &rtBright);
	glBindTexture(GL_TEXTURE_2D, rtBright);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0); // 0 == MIPMAP_BASE_LEVEL
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4); // 0 == MIPMAP_MAX_LEVEL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth / 2,  screenHeight / 2,  0, GL_RGBA, GL_FLOAT, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, screenWidth / 4,  screenHeight / 4,  0, GL_RGBA, GL_FLOAT, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA16F, screenWidth / 8,  screenHeight / 8,  0, GL_RGBA, GL_FLOAT, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA16F, screenWidth / 16, screenHeight / 16, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 4, GL_RGBA16F, screenWidth / 32, screenHeight / 32, 0, GL_RGBA, GL_FLOAT, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	// bloom mipmap
	if (rtBloomH != 0) { glDeleteTextures(1, &rtBloomH); }
	glGenTextures(1, &rtBloomH);
	glBindTexture(GL_TEXTURE_2D, rtBloomH);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0); // 0 == MIPMAP_BASE_LEVEL
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4); // 0 == MIPMAP_MAX_LEVEL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth / 2, screenHeight / 2, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, screenWidth / 4, screenHeight / 4, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA16F, screenWidth / 8, screenHeight / 8, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA16F, screenWidth / 16, screenHeight / 16, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 4, GL_RGBA16F, screenWidth / 32, screenHeight / 32, 0, GL_RGBA, GL_FLOAT, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	fboBloom1.bind();
	fboBloom1.addColorAttachment(0, rtBright, 0);
	fboBloom1.addColorAttachment(1, rtBloomH, 0);
	fboBloom1.checkStatus();
	fboBloom1.unbind();

	fboBloom2.bind();
	fboBloom2.addColorAttachment(0, rtBright, 1);
	fboBloom2.addColorAttachment(1, rtBloomH, 1);
	fboBloom2.checkStatus();
	fboBloom2.unbind();

	fboBloom3.bind();
	fboBloom3.addColorAttachment(0, rtBright, 2);
	fboBloom3.addColorAttachment(1, rtBloomH, 2);
	fboBloom3.checkStatus();
	fboBloom3.unbind();

	fboBloom4.bind();
	fboBloom4.addColorAttachment(0, rtBright, 3);
	fboBloom4.addColorAttachment(1, rtBloomH, 3);
	fboBloom4.checkStatus();
	fboBloom4.unbind();

	fboBloom5.bind();
	fboBloom5.addColorAttachment(0, rtBright, 4);
	fboBloom5.addColorAttachment(1, rtBloomH, 4);
	fboBloom5.checkStatus();
	fboBloom5.unbind();
}

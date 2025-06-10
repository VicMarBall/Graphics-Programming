#include "water.h"

void WaterResources::Init(u32 planeIdx, const int& screenWidth, const int& screenHeight)
{
	glGenTextures(1, &rtReflection);
	glBindTexture(GL_TEXTURE_2D, rtReflection);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glGenTextures(1, &rtRefraction);
	glBindTexture(GL_TEXTURE_2D, rtRefraction);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);


	glGenTextures(1, &rtReflectionDepth);
	glBindTexture(GL_TEXTURE_2D, rtReflectionDepth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glGenTextures(1, &rtRefractionDepth);
	glBindTexture(GL_TEXTURE_2D, rtRefractionDepth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


	fboReflection.bind();
	fboReflection.addColorAttachment(GL_DEPTH_ATTACHMENT, rtReflectionDepth);
	fboReflection.addColorAttachment(GL_COLOR_ATTACHMENT0, rtReflection, 0);
	fboReflection.checkStatus();
	fboReflection.unbind();

	fboRefraction.bind();
	fboRefraction.addColorAttachment(GL_DEPTH_ATTACHMENT, rtRefractionDepth);
	fboRefraction.addColorAttachment(GL_COLOR_ATTACHMENT0, rtRefraction, 0);
	fboRefraction.checkStatus();
	fboRefraction.unbind();


	waterObj.transform.setScale(vec3(10.0f, 10.0f, 10.0f));
	waterObj.transform.setPosition(vec3(0, 0, 0));
	waterObj.transform.setRotation(vec3(-90, 0, 0));

	waterObj.modelID = planeIdx;
	waterObj.deferredProgramID = waterProgramIdx;
	waterObj.forwardProgramID = waterProgramIdx;
	waterObj.forwardClipProgramID = waterProgramIdx;
}
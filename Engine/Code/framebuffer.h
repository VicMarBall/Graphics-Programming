#pragma once
#include "platform.h"
#include "resources.h"

struct FramebufferObject 
{
	FramebufferObject() { glGenFramebuffers(1, &handle); }
	~FramebufferObject() { glDeleteFramebuffers(1, &handle); }

	void bind() { glBindFramebuffer(GL_FRAMEBUFFER, handle); }
	void unbind() {	glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void addColorAttachment(GLenum attachment, GLuint texture, GLint level = 0)
	{ glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, level);	}

	void checkStatus()
	{
		GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			switch (frameBufferStatus)
			{
			case GL_FRAMEBUFFER_UNDEFINED:						ELOG("GL_FRAMEBUFFER_UNDEFINED"); break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:			ELOG("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	ELOG("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:			ELOG("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:			ELOG("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
			case GL_FRAMEBUFFER_UNSUPPORTED:					ELOG("GL_FRAMEBUFFER_UNSUPPORTED"); break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:			ELOG("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:		ELOG("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
			default: ELOG("Unknown framebuffer status error"); break;
			}
		}
	}

	GLuint handle;
};
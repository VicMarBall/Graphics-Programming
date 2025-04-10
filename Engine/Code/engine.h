//
// engine.h: This file contains the types and functions relative to the engine.
//

#pragma once

#include "platform.h"
#include "scene.h"
#include "buffer.h"
#include "resources.h"
#include <glad/glad.h>

enum FramebufferType
{
	FINAL,
	ALBEDO,
	NORMAL,
};

struct App
{
	// Loop
	f32  deltaTime;
	bool isRunning;

	// Input
	Input input;

	// Graphics
	char gpuName[64];
	char openGlVersion[64];

	ivec2 displaySize;

	// program indices
	u32 texturedGeometryProgramIdx;

	// texture indices
	u32 diceTexIdx;
	u32 whiteTexIdx;
	u32 blackTexIdx;
	u32 normalTexIdx;
	u32 magentaTexIdx;

	// scene
	Scene scene;
	
	int uniformBlockAlignment;

	// Mode
	Mode mode;

	// Embedded geometry (in-editor simple meshes such as
	// a screen filling quad, a cube, a sphere...)
	GLuint embeddedVertices;
	GLuint embeddedElements;

	// Location of the texture uniform in the textured quad shader
	GLuint programUniformTextureAlbedo;
	GLuint programUniformTextureNormals;
	GLuint texturedMeshProgram_uTexture;

	// VAO object to link our screen filling quad with our textured quad shader
	GLuint quadVAO;

	// transformation matrices
	glm::mat4 projection;
	glm::mat4 view;

	Buffer uniformsBuffer;
	u32 globalUniformHead;
	u32 globalUniformSize;

	// framebuffer
	GLuint framebufferHandle;

	// attachments
	GLuint colorAttachmentHandle;
	GLuint normalAttachmentHandle;
	GLuint depthAttachmentHandle;

	// info about OpenGL
	const unsigned char* glVersion;
	const unsigned char* glRenderer;
	const unsigned char* glVendor;
	const unsigned char* glShadingLanguageVersion;
	int glNumExtensions;
	const unsigned char* glExtensions;

	// imgui UI
	bool UIshowInfo;
	FramebufferType framebufferToDisplay;

	// resources
	std::vector<Texture>  textures;
	std::vector<Material> materials;
	std::vector<Mesh>     meshes;
	std::vector<Model>    models;
	std::vector<Program>  programs;
};

void Init(App* app);

void Gui(App* app);

void Update(App* app);

void Render(App* app);

u32 LoadTexture2D(App* app, const char* filepath);

void CreateFrameBuffers(App* app);
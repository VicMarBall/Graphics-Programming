//
// engine.cpp : Put all your graphics stuff in this file. This is kind of the graphics module.
// In here, you should type all your OpenGL commands, and you can also type code to handle
// input platform events (e.g to move the camera or react to certain shortcuts), writing some
// graphics related GUI options, and so on.
//

#include "engine.h"
#include "assimpImport.h"
#include "buffer.h"
#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <iostream>

GLuint CreateProgramFromSource(String programSource, const char* shaderName)
{
	GLchar  infoLogBuffer[1024] = {};
	GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
	GLsizei infoLogSize;
	GLint   success;

	char versionString[] = "#version 430\n";
	char shaderNameDefine[128];
	sprintf(shaderNameDefine, "#define %s\n", shaderName);
	char vertexShaderDefine[] = "#define VERTEX\n";
	char fragmentShaderDefine[] = "#define FRAGMENT\n";

	const GLchar* vertexShaderSource[] = {
		versionString,
		shaderNameDefine,
		vertexShaderDefine,
		programSource.str
	};
	const GLint vertexShaderLengths[] = {
		(GLint) strlen(versionString),
		(GLint) strlen(shaderNameDefine),
		(GLint) strlen(vertexShaderDefine),
		(GLint) programSource.len
	};
	const GLchar* fragmentShaderSource[] = {
		versionString,
		shaderNameDefine,
		fragmentShaderDefine,
		programSource.str
	};
	const GLint fragmentShaderLengths[] = {
		(GLint) strlen(versionString),
		(GLint) strlen(shaderNameDefine),
		(GLint) strlen(fragmentShaderDefine),
		(GLint) programSource.len
	};

	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
	glCompileShader(vshader);
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
		ELOG("glCompileShader() failed with vertex shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
	}

	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
		ELOG("glCompileShader() failed with fragment shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
	}

	GLuint programHandle = glCreateProgram();
	glAttachShader(programHandle, vshader);
	glAttachShader(programHandle, fshader);
	glLinkProgram(programHandle);
	glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
		ELOG("glLinkProgram() failed with program %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
	}

	glUseProgram(0);

	glDetachShader(programHandle, vshader);
	glDetachShader(programHandle, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	return programHandle;
}

u32 LoadProgram(App* app, const char* filepath, const char* programName)
{
	String programSource = ReadTextFile(filepath);

	Program program = {};
	program.handle = CreateProgramFromSource(programSource, programName);
	program.filepath = filepath;
	program.programName = programName;
	program.lastWriteTimestamp = GetFileLastWriteTimestamp(filepath);

	// get program's vertex buffer layout
	VertexBufferLayout vertexBufferLayout;
	vertexBufferLayout.stride = 0;
	int attributeCount;
	glGetProgramiv(program.handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);
	for (int i = 0; i < attributeCount; ++i) {
		char attributeName[50];
		int attributeNameLength;
		GLint attributeSize;
		GLenum attributeType;

		int attributeByteSize = sizeof(GLfloat);

		glGetActiveAttrib(program.handle, i, ARRAY_COUNT(attributeName), &attributeNameLength, &attributeSize, &attributeType, attributeName);

		if (attributeType == GL_FLOAT_VEC2) {
			attributeSize = 2;
		}
		if (attributeType == GL_FLOAT_VEC3) {
			attributeSize = 3;
		}

		int attributeLocation = glGetAttribLocation(program.handle, attributeName);

		vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ (u8)attributeLocation, (u8)attributeSize, (u8)vertexBufferLayout.stride });

		vertexBufferLayout.stride += attributeSize * attributeByteSize;
	}
	program.vertexInputLayout = vertexBufferLayout;

	app->programs.push_back(program);

	return app->programs.size() - 1;
}

Image LoadImage(const char* filename)
{
	Image img = {};
	stbi_set_flip_vertically_on_load(true);
	img.pixels = stbi_load(filename, &img.size.x, &img.size.y, &img.nchannels, 0);
	if (img.pixels)
	{
		img.stride = img.size.x * img.nchannels;
	}
	else
	{
		ELOG("Could not open file %s", filename);
	}
	return img;
}

void FreeImage(Image image)
{
	stbi_image_free(image.pixels);
}

GLuint CreateTexture2DFromImage(Image image)
{
	GLenum internalFormat = GL_RGB8;
	GLenum dataFormat     = GL_RGB;
	GLenum dataType       = GL_UNSIGNED_BYTE;

	switch (image.nchannels)
	{
		case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
		case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
		default: ELOG("LoadTexture2D() - Unsupported number of channels");
	}

	GLuint texHandle;
	glGenTextures(1, &texHandle);
	glBindTexture(GL_TEXTURE_2D, texHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size.x, image.size.y, 0, dataFormat, dataType, image.pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texHandle;
}

u32 LoadTexture2D(App* app, const char* filepath)
{
	for (u32 texIdx = 0; texIdx < app->textures.size(); ++texIdx)
		if (app->textures[texIdx].filepath == filepath)
			return texIdx;

	Image image = LoadImage(filepath);

	if (image.pixels)
	{
		Texture tex = {};
		tex.handle = CreateTexture2DFromImage(image);
		tex.filepath = filepath;

		u32 texIdx = app->textures.size();
		app->textures.push_back(tex);

		FreeImage(image);
		return texIdx;
	}
	else
	{
		return UINT32_MAX;
	}
}

void CreateFrameBuffers(App* app)
{
	// color
	glGenTextures(1, &app->colorAttachmentHandle);
	glBindTexture(GL_TEXTURE_2D, app->colorAttachmentHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, app->displaySize.x, app->displaySize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// normal
	glGenTextures(1, &app->normalAttachmentHandle);
	glBindTexture(GL_TEXTURE_2D, app->normalAttachmentHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, app->displaySize.x, app->displaySize.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// position
	glGenTextures(1, &app->positionAttachmentHandle);
	glBindTexture(GL_TEXTURE_2D, app->positionAttachmentHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, app->displaySize.x, app->displaySize.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// depth
	glGenTextures(1, &app->depthAttachmentHandle);
	glBindTexture(GL_TEXTURE_2D, app->depthAttachmentHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, app->displaySize.x, app->displaySize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);


	// framebuffer object (FBO)
	glGenFramebuffers(1, &app->framebufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, app->framebufferHandle);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, app->colorAttachmentHandle, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, app->normalAttachmentHandle, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, app->positionAttachmentHandle, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, app->depthAttachmentHandle, 0);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

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

	glDrawBuffers(ARRAY_COUNT(buffers), buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FindVAO(Mesh& mesh, u32 submeshIndex, const Program& program) {
	Submesh& submesh = mesh.submeshes[submeshIndex];

	// try finding a VAO for this submesh / program
	for (u32 i = 0; i < (u32)submesh.vaos.size(); ++i) {
		if (submesh.vaos[i].programHandle == program.handle) {
			return submesh.vaos[i].handle;
		}
	}

	GLuint vaoHandle = 0;

	// create a new VAO for this submesh / program
	{
		glGenVertexArrays(1, &vaoHandle);
		glBindVertexArray(vaoHandle);

		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferHandle);

		// link all vertex input attributes to attributes in the vertex buffer
		for (u32 i = 0; i < program.vertexInputLayout.attributes.size(); ++i) 
		{
			bool attributeIsLinked = false;

			for (u32 j = 0; j < submesh.vertexBufferLayout.attributes.size(); ++j) 
			{
				if (program.vertexInputLayout.attributes[i].location == submesh.vertexBufferLayout.attributes[j].location)
				{
					const u32 index = submesh.vertexBufferLayout.attributes[j].location;
					const u32 numComponents = submesh.vertexBufferLayout.attributes[j].componentCount;
					const u32 offset = submesh.vertexBufferLayout.attributes[j].offset + submesh.vertexOffset;
					const u32 stride = submesh.vertexBufferLayout.stride;

					glVertexAttribPointer(index, numComponents, GL_FLOAT, GL_FALSE, stride, (void*)(u64)offset);
					glEnableVertexAttribArray(index);

					attributeIsLinked = true;
					break;

				}
			}

			assert(attributeIsLinked);
		}

		glBindVertexArray(0);
	}

	// store vao into the list of vaos for this submesh
	VAO vao = { vaoHandle, program.handle };
	submesh.vaos.push_back(vao);

	return vaoHandle;
}

void Init(App* app)
{
	app->framebufferToDisplay = FramebufferType::FINAL;
	app->UIshowInfo = false;

	app->glVersion = glGetString(GL_VERSION);
	app->glRenderer = glGetString(GL_RENDERER);
	app->glVendor = glGetString(GL_VENDOR);
	app->glShadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	glGetIntegerv(GL_NUM_EXTENSIONS, &app->glNumExtensions);
	for (int i = 0; i < app->glNumExtensions; ++i) {
		app->glExtensions = glGetStringi(GL_EXTENSIONS, GLuint(i));
	}

	CreateFrameBuffers(app);

	app->scene.camera.transform.translate(vec3(0.0f, 0.0f, -10.0f));

	// TODO: Initialize your resources here!
	// - vertex buffers
	// - element/index buffers
	// - vaos
	// - programs (and retrieve uniform indices)
	// - textures

	// screen quad
	{
		float vertices[] =
		{
			-1,	-1,	0,	0.0,	0.0, // bottom-left
			1,	-1,	0,	1.0,	0.0, // bottom-right
			1,	1,	0,	1.0,	1.0, // top-right
			-1,	1,	0,	0.0,	1.0, // top-left
		};

		const u16 indexes[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		VertexBufferLayout vertexBufferLayout;
		vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });					// 3D positions
		vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 2, 3 * sizeof(float) });	// tex coordinates
		vertexBufferLayout.stride = 5 * sizeof(float);


		// geometry
		glGenBuffers(1, &app->embeddedVertices);
		glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &app->embeddedElements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// attributes
		glGenVertexArrays(1, &app->quadVAO);
		glBindVertexArray(app->quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);

		for (int i = 0; i < vertexBufferLayout.attributes.size(); ++i) {
			glVertexAttribPointer(vertexBufferLayout.attributes[i].location, vertexBufferLayout.attributes[i].componentCount, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)vertexBufferLayout.attributes[i].offset);
			glEnableVertexAttribArray(vertexBufferLayout.attributes[i].location);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
		glBindVertexArray(0);

		app->screenQuadProgramIdx = LoadProgram(app, "shaders.glsl", "SCREEN_QUAD");
		Program& texturedGeometryProgram = app->programs[app->screenQuadProgramIdx];
		app->programCurrentFramebufferLocation = glGetUniformLocation(texturedGeometryProgram.handle, "usedFramebuffer");
		app->programUniformTextureAlbedo = glGetUniformLocation(texturedGeometryProgram.handle, "uAlbedo");
		app->programUniformTextureNormals = glGetUniformLocation(texturedGeometryProgram.handle, "uNormals");
		app->programUniformTexturePosition = glGetUniformLocation(texturedGeometryProgram.handle, "uPosition");
		app->programUniformTextureDepth = glGetUniformLocation(texturedGeometryProgram.handle, "uDepth");
	}

	// mesh
	{
		app->scene.gameObjects.push_back(GameObject());
		GameObject& gameObject = app->scene.gameObjects.back();

		// geometry
		u32 modelID = LoadModel(app, "Patrick/patrick.obj");
		gameObject.modelID = modelID;

		// program
		u32 programID = LoadProgram(app, "shaders.glsl", "TEXTURED_MESH");
		gameObject.programID = programID;

		app->scene.gameObjects.push_back(GameObject());
		GameObject& gameObject2 = app->scene.gameObjects.back();

		gameObject2.modelID = modelID;
		gameObject2.programID = programID;

		gameObject2.transform.translate(vec3(5.0f, 0.0f, 0.0f));

		app->scene.gameObjects.push_back(GameObject());
		GameObject& bakerHouse = app->scene.gameObjects.back();

		// geometry
		u32 bakerHouseModelID = LoadModel(app, "Baker House/BakerHouse.fbx");
		bakerHouse.modelID = bakerHouseModelID;

		bakerHouse.programID = programID;

		bakerHouse.transform.translate(vec3(-5.0f, 0.0f, 0.0f));
		bakerHouse.transform.scale(vec3(0.01f, 0.01f, 0.01f));
	}

	int maxUniformBufferSize;
	//int uniformBlockAlignment;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBufferSize);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &app->uniformBlockAlignment);
	

	Light light;
	light.type = LightType_Point;
	light.color = vec3(1.0f, 1.0f, 1.0f);
	light.direction = vec3(0.0f, -1.0f, 0.0f);
	light.position = vec3(0.0f, 2.0f, -1.0f);

	app->scene.lights.push_back(light);

	
	// for each buffer you need

	app->uniformsBuffer = CreateBuffer(maxUniformBufferSize, GL_UNIFORM_BUFFER, GL_STREAM_DRAW);

	//glGenBuffers(1, &app->globalUniformBuffer.handle);
	//glBindBuffer(GL_UNIFORM_BUFFER, app->globalUniformBuffer.handle);
	//glBufferData(GL_UNIFORM_BUFFER, maxUniformBufferSize, NULL, GL_STREAM_DRAW);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//glGenBuffers(1, &app->uniformsBufferHandle);
	//glBindBuffer(GL_UNIFORM_BUFFER, app->uniformsBufferHandle);
	//glBufferData(GL_UNIFORM_BUFFER, maxUniformBufferSize, NULL, GL_STREAM_DRAW);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Gui(App* app)
{
	if (app->UIshowInfo) {
		ImGui::Begin("Info", &app->UIshowInfo);

		ImGui::Text("FPS: %f", 1.0f / app->deltaTime);
		ImGui::Text("OpenGL Version: %s", app->glVersion);
		ImGui::Text("OpenGL Renderer: %s", app->glRenderer);
		ImGui::Text("OpenGL Vendor: %s", app->glVendor);
		ImGui::Text("OpenGL Shading Language Version: %s", app->glShadingLanguageVersion);
		ImGui::Text("OpenGL Number of Extensions: %d", app->glNumExtensions);
		ImGui::Text("OpenGL Extensions: %s", app->glExtensions);

		ImGui::End();
	}


	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("General")) {
			
			if (ImGui::MenuItem("Info")) { app->UIshowInfo = true; }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Display")) {

			if (ImGui::MenuItem("Final")) { app->framebufferToDisplay = FramebufferType::FINAL; }
			if (ImGui::MenuItem("Albedo")) { app->framebufferToDisplay = FramebufferType::ALBEDO; }
			if (ImGui::MenuItem("Normals")) { app->framebufferToDisplay = FramebufferType::NORMAL; }
			if (ImGui::MenuItem("Position")) { app->framebufferToDisplay = FramebufferType::POSITION; }
			if (ImGui::MenuItem("Lights")) { app->framebufferToDisplay = FramebufferType::LIGHTS; }
			if (ImGui::MenuItem("Depth")) { app->framebufferToDisplay = FramebufferType::DEPTH; }

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void Update(App* app)
{
	// You can handle app->input keyboard/mouse here
	float cameraSpeed = 0.1f;
	// camera translation
	if (app->input.keys[K_W]) { app->scene.camera.transform.translate(vec3( 0.0f, 0.0f,  1.0f) * cameraSpeed); }
	if (app->input.keys[K_A]) { app->scene.camera.transform.translate(vec3( 1.0f, 0.0f,  0.0f) * cameraSpeed); }
	if (app->input.keys[K_S]) { app->scene.camera.transform.translate(vec3( 0.0f, 0.0f, -1.0f) * cameraSpeed); }
	if (app->input.keys[K_D]) { app->scene.camera.transform.translate(vec3(-1.0f, 0.0f,  0.0f) * cameraSpeed); }

	// camera rotation
	{
		float cameraRotationSpeed = 0.1f;
		if (app->input.mouseButtons[RIGHT]) {
			app->scene.camera.transform.rotate(cameraRotationSpeed * -app->input.mouseDelta.x, vec3(0.0f, 1.0f, 0.0f), GLOBAL);
			app->scene.camera.transform.rotate(cameraRotationSpeed * app->input.mouseDelta.y, vec3(1.0f, 0.0f, 0.0f));
		}
	}



	// when minimizing the window, displaySize.y == 0 -> displaySize.x/0 -> error
	if (app->displaySize.y > 0) {
		float aspectRatio = (float)app->displaySize.x / (float)app->displaySize.y;
		app->projection = glm::perspective(glm::radians(60.0f), aspectRatio, app->scene.camera.zNear, app->scene.camera.zFar);
		app->view = glm::lookAt(app->scene.camera.transform.getPosition(), app->scene.camera.transform.getPosition() + app->scene.camera.transform.getForward(), app->scene.camera.transform.getUp());
	}

	GameObject& gameObject = app->scene.gameObjects.back();
	gameObject.transform.rotate(1.0f, vec3(0.0f, 1.0f, 0.0f));

	// global uniforms
	app->globalUniformHead = app->uniformsBuffer.head;

	MapBuffer(app->uniformsBuffer, GL_WRITE_ONLY);

	PushVec3(app->uniformsBuffer, app->scene.camera.transform.getPosition());
	PushUInt(app->uniformsBuffer, app->scene.lights.size());

	for (u32 i = 0; i < app->scene.lights.size(); ++i) 
	{
		AlignHead(app->uniformsBuffer, sizeof(vec4));

		Light& light = app->scene.lights[i];
		PushUInt(app->uniformsBuffer, light.type);
		PushVec3(app->uniformsBuffer, light.color);
		PushVec3(app->uniformsBuffer, light.direction);
		PushVec3(app->uniformsBuffer, light.position);
	}

	app->globalUniformSize = app->uniformsBuffer.head - app->globalUniformHead;


	// prepare the local uniforms
	for (GameObject& gameObject : app->scene.gameObjects) 
	{
		AlignHead(app->uniformsBuffer, app->uniformBlockAlignment);

		glm::mat4 worldViewProjectionMatrix = app->projection * app->view * gameObject.transform.getTransform();

		gameObject.localUniformBufferHead = app->uniformsBuffer.head;
		PushMat4(app->uniformsBuffer, gameObject.transform.getTransform());
		PushMat4(app->uniformsBuffer, worldViewProjectionMatrix);
		gameObject.localUniformBufferSize = app->uniformsBuffer.head - gameObject.localUniformBufferHead;
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Render(App* app)
{
	// render on this framebuffer render targets
	glBindFramebuffer(GL_FRAMEBUFFER, app->framebufferHandle);

	// select on which render target to draw
	GLuint drawBuffers[] = { app->colorAttachmentHandle, app->normalAttachmentHandle };
	glDrawBuffers(ARRAY_COUNT(drawBuffers), drawBuffers);

	// clear color and depth
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, app->displaySize.x, app->displaySize.y);

	glEnable(GL_DEPTH_TEST);

	
	// render meshes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, app->uniformsBuffer.handle, app->globalUniformHead, app->globalUniformSize);

	for (const GameObject& gameObject : app->scene.gameObjects)
	{
		// set the block of the uniform
		u32 blockOffset = gameObject.localUniformBufferHead;
		u32 blockSize = gameObject.localUniformBufferSize;
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, app->uniformsBuffer.handle, blockOffset, blockSize);

		// use the program
		Program& texturedMeshProgram = app->programs[gameObject.programID];
		glUseProgram(texturedMeshProgram.handle);

		// draw the mesh
		Model& model = app->models[gameObject.modelID];
		Mesh& mesh = app->meshes[model.meshIdx];

		for (u32 i = 0; i < mesh.submeshes.size(); ++i) {
			GLuint vao = FindVAO(mesh, i, texturedMeshProgram);
			glBindVertexArray(vao);

			u32 submeshMaterialIdx = model.materialIdx[i];
			Material& submeshMaterial = app->materials[submeshMaterialIdx];

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, app->textures[submeshMaterial.albedoTextureIdx].handle);
			glUniform1i(app->texturedMeshProgram_uTexture, 0);

			Submesh& submesh = mesh.submeshes[i];
			glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, (void*)(u64)submesh.indexOffset);
		}
	}
	
	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render plane on the viewport to put the texture form the framebuffer
	Program& programTexturedGeometry = app->programs[app->screenQuadProgramIdx];
	glUseProgram(programTexturedGeometry.handle);
	glBindVertexArray(app->quadVAO);

	glDisable(GL_DEPTH_TEST);

	// clear color and depth
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniform1ui(app->programCurrentFramebufferLocation, app->framebufferToDisplay);

	GLuint textureHandle;

	// albedo
	glUniform1i(app->programUniformTextureAlbedo, 0);
	glActiveTexture(GL_TEXTURE0);
	textureHandle = app->colorAttachmentHandle;
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	// normals
	glUniform1i(app->programUniformTextureNormals, 1);
	glActiveTexture(GL_TEXTURE1);
	textureHandle = app->normalAttachmentHandle;
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	// position
	glUniform1i(app->programUniformTexturePosition, 2);
	glActiveTexture(GL_TEXTURE2);
	textureHandle = app->positionAttachmentHandle;
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	// depth
	glUniform1i(app->programUniformTextureDepth, 3);
	glActiveTexture(GL_TEXTURE3);
	textureHandle = app->depthAttachmentHandle;
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

}


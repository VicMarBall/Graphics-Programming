//
// engine.cpp : Put all your graphics stuff in this file. This is kind of the graphics module.
// In here, you should type all your OpenGL commands, and you can also type code to handle
// input platform events (e.g to move the camera or react to certain shortcuts), writing some
// graphics related GUI options, and so on.
//

#include "engine.h"
#include "assimpImport.h"
#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>

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
				const u32 index = submesh.vertexBufferLayout.attributes[j].location;
				const u32 numComponents = submesh.vertexBufferLayout.attributes[j].componentCount;
				const u32 offset = submesh.vertexBufferLayout.attributes[j].offset + submesh.vertexOffset;
				const u32 stride = submesh.vertexBufferLayout.stride;

				glVertexAttribPointer(index, numComponents, GL_FLOAT, GL_FALSE, stride, (void*)(u64)offset);
				glEnableVertexAttribArray(index);
				
				attributeIsLinked = true;
				break;
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
	app->glVersion = glGetString(GL_VERSION);
	app->glRenderer = glGetString(GL_RENDERER);
	app->glVendor = glGetString(GL_VENDOR);
	app->glShadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	glGetIntegerv(GL_NUM_EXTENSIONS, &app->glNumExtensions);
	for (int i = 0; i < app->glNumExtensions; ++i) {
		app->glExtensions = glGetStringi(GL_EXTENSIONS, GLuint(i));
	}


	// TODO: Initialize your resources here!
	// - vertex buffers
	// - element/index buffers
	// - vaos
	// - programs (and retrieve uniform indices)
	// - textures

	app->mode = Mode_TexturedMesh;

	float vertices[] =
	{
		-0.5,	-0.5,	0,	0.0,	0.0, // bottom-left
		0.5,	-0.5,	0,	1.0,	0.0, // bottom-right
		0.5,	0.5,	0,	1.0,	1.0, // top-right
		-0.5,	0.5,	0,	0.0,	1.0, // top-left
	};

	const u16 indexes[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	//vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });					// 3D positions
	//vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, 3 * sizeof(float) });	// tex coordinates
	//vertexBufferLayout.stride = 5 * sizeof(float);


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
	glGenVertexArrays(1, &app->vao);
	glBindVertexArray(app->vao);
	glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)12);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
	glBindVertexArray(0);

	app->texturedGeometryProgramIdx = LoadProgram(app, "shaders.glsl", "TEXTURED_GEOMETRY");
	Program& texturedGeometryProgram = app->programs[app->texturedGeometryProgramIdx];
	app->programUniformTexture = glGetUniformLocation(texturedGeometryProgram.handle, "uTexture");

	// geometry
	app->modelIdx = LoadModel(app, "Patrick/patrick.obj");

	// attributes
	glGenVertexArrays(1, &app->vao);
	glBindVertexArray(app->vao);
	glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)12);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
	glBindVertexArray(0);


	app->texturedMeshProgramIdx = LoadProgram(app, "shaders.glsl", "TEXTURED_MESH");
	Program& texturedMeshProgram = app->programs[app->texturedMeshProgramIdx];
	app->texturedMeshProgram_uTexture = glGetUniformLocation(texturedMeshProgram.handle, "uTexture");

	// get vertex buffer layout
	VertexBufferLayout vertexBufferLayout;
	int attributeCount;
	glGetProgramiv(texturedMeshProgram.handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);
	int attributeOffset = 0;
	for (int i = 0; i < attributeCount; ++i) {
		char attributeName[50];
		int attributeNameLength;
		GLint attributeSize;
		GLenum attributeType;

		int attributeByteSize = sizeof(GLfloat);

		glGetActiveAttrib(texturedMeshProgram.handle, i, ARRAY_COUNT(attributeName), &attributeNameLength, &attributeSize, &attributeType, attributeName);

		if (attributeType == GL_FLOAT_VEC2) {
			attributeByteSize = sizeof(GLfloat) * 2;
		}
		if (attributeType == GL_FLOAT_VEC3) {
			attributeByteSize = sizeof(GLfloat) * 3;
		}

		int attributeLocation = glGetAttribLocation(texturedMeshProgram.handle, attributeName);

		vertexBufferLayout.attributes.push_back(VertexBufferAttribute{(u8)attributeLocation, (u8)attributeSize, (u8)attributeOffset});

		attributeOffset += attributeByteSize * attributeSize;
	}
	vertexBufferLayout.stride = attributeOffset;

	//vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });					// 3D positions
	//vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, 3 * sizeof(float) });	// tex coordinates
	//vertexBufferLayout.stride = 5 * sizeof(float);

	texturedMeshProgram.vertexInputLayout = vertexBufferLayout;


	app->diceTexIdx = LoadTexture2D(app, "dice.png");
	app->whiteTexIdx = LoadTexture2D(app, "color_white.png");
	app->blackTexIdx = LoadTexture2D(app, "color_black.png");
	app->normalTexIdx = LoadTexture2D(app, "color_normal.png");
	app->magentaTexIdx = LoadTexture2D(app, "color_magenta.png");

}

void Gui(App* app)
{
	ImGui::Begin("Info");
	ImGui::Text("FPS: %f", 1.0f / app->deltaTime);
	ImGui::Text("OpenGL Version: %s", app->glVersion);
	ImGui::Text("OpenGL Renderer: %s", app->glRenderer);
	ImGui::Text("OpenGL Vendor: %s", app->glVendor);
	ImGui::Text("OpenGL Shading Language Version: %s", app->glShadingLanguageVersion);
	ImGui::Text("OpenGL Number of Extensions: %d", app->glNumExtensions);
	ImGui::Text("OpenGL Extensions: %s", app->glExtensions);

	ImGui::End();
}

void Update(App* app)
{
	// You can handle app->input keyboard/mouse here
}

void Render(App* app)
{
	switch (app->mode)
	{
		case Mode_TexturedQuad:
		{
			// TODO: Draw your textured quad here!
			// - clear the framebuffer
			// - set the viewport
			// - set the blending state
			// - bind the texture into unit 0
			// - bind the program 
			//   (...and make its texture sample from unit 0)
			// - bind the vao
			// - glDrawElements() !!!

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glViewport(0, 0, app->displaySize.x, app->displaySize.y);

			Program& programTexturedGeometry = app->programs[app->texturedGeometryProgramIdx];
			glUseProgram(programTexturedGeometry.handle);
			glBindVertexArray(app->vao);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUniform1i(app->programUniformTexture, 0);
			glActiveTexture(GL_TEXTURE0);
			GLuint textureHandle = app->textures[app->diceTexIdx].handle;
			glBindTexture(GL_TEXTURE_2D, textureHandle);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

			glBindVertexArray(0);
			glUseProgram(0);
			
		break;
		}
		case Mode_TexturedMesh:
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glViewport(0, 0, app->displaySize.x, app->displaySize.y);

			Program& texturedMeshProgram = app->programs[app->texturedMeshProgramIdx];
			glUseProgram(texturedMeshProgram.handle);

			Model& model = app->models[app->modelIdx];
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

			break;
		}
		default:;
	}
}


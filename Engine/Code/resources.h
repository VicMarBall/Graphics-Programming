#pragma once

#include "platform.h"
#include <glad/glad.h>

typedef glm::vec2  vec2;
typedef glm::vec3  vec3;
typedef glm::vec4  vec4;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;

struct Image
{
	void* pixels;
	ivec2 size;
	i32   nchannels;
	i32   stride;
};

struct Texture
{
	GLuint      handle;
	std::string filepath;
};

enum Mode
{
	Mode_TexturedQuad,
	Mode_Count,
	Mode_TexturedMesh
};

struct VertexV3V2
{
	glm::vec3 pos;
	glm::vec2 uv;
};

struct VertexBufferAttribute
{
	u8 location;
	u8 componentCount;
	u8 offset;
};

struct VertexBufferLayout
{
	std::vector<VertexBufferAttribute>  attributes;
	u8                                  stride;
};

struct VAO
{
	GLuint handle;
	GLuint programHandle;
};

struct VertexShaderAttribute
{
	u8 location;
	u8 componentCount;
};

struct VertexShaderLayout
{
	std::vector<VertexShaderAttribute> attributes;
};

struct Submesh
{
	VertexBufferLayout  vertexBufferLayout;
	std::vector<float>  vertices;
	std::vector<u32>    indices;
	u32                 vertexOffset;
	u32                 indexOffset;

	std::vector<VAO>    vaos;
};

struct Mesh
{
	std::vector<Submesh>    submeshes;
	GLuint                  vertexBufferHandle;
	GLuint					indexBufferHandle;
};

struct Model
{
	u32				 meshIdx;
	std::vector<u32> materialIdx;
};

struct Material
{
	std::string name;
	vec3		albedo;
	vec3		emissive;
	f32			smoothness;
	u32			albedoTextureIdx;
	u32			emissiveTextureIdx;
	u32			specularTextureIdx;
	u32			normalsTextureIdx;
	u32			bumpTextureIdx;
};

struct Program
{
	GLuint             handle;
	std::string        filepath;
	std::string        programName;
	u64                lastWriteTimestamp; // What is this for?
	VertexBufferLayout vertexInputLayout;
};
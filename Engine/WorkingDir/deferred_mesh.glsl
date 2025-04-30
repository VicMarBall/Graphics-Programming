
///////////////////////////////////////////////////////////////////////

struct Light
{
	unsigned int type;
	vec3 color;
	vec3 direction;
	vec3 position;
};

///////////////////////////////////////////////////////////////////////

#ifdef TEXTURED_MESH

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
//layout(location = 3) in vec3 aTangent;
//layout(location = 4) in vec3 aBitangent;

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

out vec2 vTexCoord;
out vec3 vPosition; // in worldspace
out vec3 vNormal;   // in worldspace

void main()
{
	vTexCoord = aTexCoord;

	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = normalize(vec3(uWorldMatrix * vec4(aNormal, 0.0)));

	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;

uniform sampler2D uTexture;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[256];
};

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oPosition;

void main()
{
//	vec4 baseColor = texture(uTexture, vTexCoord);
//	vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);

//	for (int i = 0; i < uLightCount; ++i)
//	{
//		// directional
//		lightColor += dot(vNormal, normalize(uLight[i].direction)) * uLight[i].color;
//	}

//	oColor = baseColor * vec4(lightColor, 1.0f);

	oColor = texture(uTexture, vTexCoord);
	oNormal = vec4(vNormal, 1.0);
	oPosition = vec4(vPosition, 1.0);

}

#endif
#endif

///////////////////////////////////////////////////////////////////////

#ifdef BASIC_SHAPE

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

out vec3 vPosition; // in worldspace
out vec3 vNormal;   // in worldspace

void main()
{
	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = normalize(vec3(uWorldMatrix * vec4(aNormal, 0.0)));

	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec3 vPosition;
in vec3 vNormal;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[256];
};

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oPosition;

void main()
{

	oColor = vec4(1.0, 1.0, 1.0, 1.0);
	oNormal = vec4(vNormal, 1.0);
	oPosition = vec4(vPosition, 1.0);

}

#endif
#endif

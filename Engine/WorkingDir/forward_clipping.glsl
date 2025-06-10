
///////////////////////////////////////////////////////////////////////

struct Light
{
	unsigned int type;
	vec3 color;
	vec3 direction;
	vec3 position;
};

///////////////////////////////////////////////////////////////////////

#ifdef CLIP_TEXTURED_MESH

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

uniform vec4 clippingPlane;
uniform vec3 eyeWorldSpace;

out vec2 vTexCoord;
out vec3 vPosition; // in worldspace
out vec3 vNormal;   // in worldspace

void main()
{
	vTexCoord = aTexCoord;

	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = normalize(vec3(uWorldMatrix * vec4(aNormal, 0.0)));

    vec4 clipDistanceDisplacement = vec4(0.0, 0.0, 0.0, length(eyeWorldSpace) / 100.0);

	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
    gl_ClipDistance[0] = dot(vec4(aPosition.zyz, 0.0), clippingPlane + clipDistanceDisplacement);
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

void main()
{
	vec4 baseColor = texture(uTexture, vTexCoord);

	vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);	
	for (int i = 0; i < uLightCount; ++i)
	{
		vec3 diffuse;
		switch (uLight[i].type)
		{
		case 0: // point light
			float distanceToPoint = distance(uLight[i].position, vPosition);
			vec3 directionVector = vPosition - uLight[i].position;
	
			diffuse = max(0.0f, -dot(vNormal, normalize(directionVector))) * uLight[i].color / distanceToPoint;
	
			lightColor += diffuse;
			break;
		case 1: // directional
			diffuse = max(0.0f, -dot(vNormal, normalize(uLight[i].direction))) * uLight[i].color;
	
			lightColor += diffuse;
			break;
		default:
			break;
		}
	}

	oColor = baseColor * vec4(lightColor, 1.0);
}

#endif
#endif

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////

#ifdef CLIP_BASIC_SHAPE

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

uniform vec4 clippingPlane;
uniform vec3 eyeWorldSpace;

out vec3 vPosition; // in worldspace
out vec3 vNormal;   // in worldspace

void main()
{
	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = normalize(vec3(uWorldMatrix * vec4(aNormal, 0.0)));

    vec4 clipDistanceDisplacement = vec4(0.0, 0.0, 0.0, length(eyeWorldSpace) / 100.0);

	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
    gl_ClipDistance[0] = dot(vec4(aPosition.zyz, 0.0), clippingPlane + clipDistanceDisplacement);
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

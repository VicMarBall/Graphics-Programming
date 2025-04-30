///////////////////////////////////////////////////////////////////////

struct Light
{
	unsigned int type;
	vec3 color;
	vec3 direction;
	vec3 position;
};

///////////////////////////////////////////////////////////////////////

#ifdef SCREEN_QUAD

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;

	gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;

uniform unsigned int usedFramebuffer;

uniform sampler2D uAlbedo;
uniform sampler2D uNormals;
uniform sampler2D uPosition;
uniform sampler2D uDepth;

layout(location = 0) out vec4 oColor;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[256];
};

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{

	vec4 baseColor = texture(uAlbedo, vTexCoord);
	vec3 normals = texture(uNormals, vTexCoord).xyz;
	vec3 position = texture(uPosition, vTexCoord).xyz;
	float depth = linearizeDepth(texture(uDepth, vTexCoord).r) / far;

	vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);	
	for (int i = 0; i < uLightCount; ++i)
	{
		vec3 diffuse;
		switch (uLight[i].type)
		{
		case 0: // point light
			float distanceToPoint = distance(uLight[i].position, position);
			vec3 directionVector = position - uLight[i].position;

			diffuse = max(0.0f, -dot(normals, normalize(directionVector))) * uLight[i].color / distanceToPoint;

			lightColor += diffuse;
			break;
		case 1: // directional
			diffuse = max(0.0f, -dot(normals, normalize(uLight[i].direction))) * uLight[i].color;

			lightColor += diffuse;
			break;
		default:
			break;
		}
	}

	switch (usedFramebuffer)
	{
	case 0: // final
		oColor = baseColor * vec4(lightColor, 1.0f);
		break;
	case 1: // albedo
		oColor = baseColor;
		break;
	case 2: // normals
		oColor = vec4(normals, 1.0f);
		break;
	case 3: // position
		oColor = vec4(position, 1.0f);
		break;
	case 4: // lights
		oColor = vec4(lightColor, 1.0f);
		break;
	case 5: // depth
		oColor = vec4(depth, depth, depth, 1.0f);
	default: 
		break;
	}
}

#endif
#endif
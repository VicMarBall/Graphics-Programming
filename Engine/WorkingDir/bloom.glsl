#ifdef BLIT_BRIGHTEST_PIXELS

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

uniform sampler2D colorTexture;
uniform float threshold;

in vec2 vTexCoord;

out vec4 outColor;

void main()
{
	vec3 luminances = vec3(0.2126, 0.7152, 0.0722);
	vec4 texel = texture2D(colorTexture, vTexCoord);
	float luminance = dot(luminances, texel.rgb);
	luminance = max(0.0, luminance - threshold);
	texel.rgb *= sign(luminance);
	texel.a = 1.0;
	outColor = texel;
}

#endif
#endif

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#ifdef BLOOM

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

uniform sampler2D colorMap;
uniform int maxLOD;

in vec2 vTexCoord;

out vec4 outColor;

void main()
{
	outColor = vec4(0.0);
	for (int lod = 0; lod < maxLOD; ++lod)
	{
		outColor += textureLod(colorMap, texCoord, float(lod));
	}
	outColor.a = 1.0;
}

#endif
#endif
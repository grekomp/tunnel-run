#version 330
#extension GL_ARB_explicit_uniform_location : enable

//-------------------------------------------------------------------
layout (location = 9) uniform sampler2D textureSamplerDiff;
layout (location = 10) uniform sampler2D textureSamplerSpec;
layout (location = 11) uniform sampler2D textureSamplerNorm;
layout (location = 12) uniform sampler2D textureSamplerDisp;

layout (location = 13) uniform vec3 materialAmbient;
layout (location = 14) uniform vec3 materialDiffuse;
layout (location = 15) uniform vec3 materialSpecular;
layout (location = 16) uniform float shininess;

layout (location = 17) uniform vec4 lightPosition;
layout (location = 18) uniform vec3 lightAmbient;
layout (location = 19) uniform vec3 lightDiffuse;
layout (location = 20) uniform vec3 lightSpecular;

layout (location = 21) uniform float parallaxStrength;

in vec2 texCoords;
in vec3 lightDir;
in vec3 viewDir;

out vec4 fColor;

vec3 lightD;
vec3 viewD;

//-------------------------------------------------------------------

vec4 ads(
	vec3 norm,
	vec3 materialAmbient,
	vec3 materialDiffuse,
	vec3 materialSpecular,
	float shininess
);

vec2 parallaxMapping(vec3 viewDir, vec2 texCoords);

//-------------------------------------------------------------------

void main()
{
	lightD = normalize(lightDir);
	viewD = normalize(viewDir);

	vec3 N = normalize(2.0 * texture(textureSamplerNorm, texCoords).rgb - 1.0);

	vec4 colorL = ads(N, materialAmbient, vec3(texture(textureSamplerDiff, texCoords)), vec3(texture(textureSamplerSpec, texCoords)), shininess);
	 
	fColor = clamp(colorL, 0.0, 1.0 );
}

// funkcja obliczajaca oswietlenie
vec4 ads(vec3 norm, vec3 materialAmbient, vec3 materialDiffuse, vec3 materialSpecular, float shininess )
{
	vec3 ambient = lightAmbient * materialAmbient;
	
	vec3 diffuse = lightDiffuse * materialDiffuse * max( dot( lightD, norm ), 0.0 );

	vec3 specular = vec3( 0.0, 0.0, 0.0 );
	vec3 refl = reflect( vec3( 0.0, 0.0, 0.0 ) - lightD, norm );
	specular = pow( max( 0.0, dot( viewD, refl ) ), shininess ) * materialSpecular * lightSpecular;

    return vec4( clamp( ambient + diffuse + specular, 0.0, 1.0 ), 1.0 );
}


#version 330
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_EXT_texture_filter_anisotropic : enable

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

layout (location = 23) uniform bool ignoreTextures;
layout (location = 24) uniform float renderDistance;

in vec2 texCoords;
in vec3 lightDir;
in vec3 viewDir;
in vec3 position;

out vec4 fColor;

vec3 lightD;
vec3 viewD;

//-------------------------------------------------------------------

void main()
{
	lightD = normalize(lightDir);
	viewD = normalize(viewDir);

	vec3 N;
	
	// Lighting
	vec3 ambient;
	vec3 diffuse;
	vec3 specular = vec3( 0.0, 0.0, 0.0 );
	vec3 refl;

	if (!ignoreTextures) {
		N = normalize(2.0 * texture(textureSamplerNorm, texCoords).rgb - 1.0);
		
		refl = reflect( vec3( 0.0, 0.0, 0.0 ) - lightD, N );

		ambient = lightAmbient * vec3(texture(textureSamplerDiff, texCoords));
		diffuse = lightDiffuse * vec3(texture(textureSamplerDiff, texCoords)) * max( dot( lightD, N ), 0.0 );
		specular = pow( max( 0.0, dot( viewD, refl ) ), shininess ) * vec3(texture(textureSamplerSpec, texCoords)) * lightSpecular;
	} else {
		N = vec3(0, 0, 1);

		refl = reflect( vec3( 0.0, 0.0, 0.0 ) - lightD, N );
		ambient = lightAmbient * materialAmbient;
		diffuse = lightDiffuse * materialDiffuse * max( dot( lightD, N ), 0.0 );
		specular = pow( max( 0.0, dot( viewD, refl ) ), shininess ) * materialSpecular * lightSpecular;
	}

	vec4 color = vec4( clamp( ambient + diffuse + specular, 0.0, 1.0 ), 1.0 );

	color = mix(color, vec4(0, 0, 0, 0), length(position) / renderDistance); 

	fColor = color;
}



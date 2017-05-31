#version 330
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_EXT_texture_filter_anisotropic : enable


layout (location = 17) uniform vec4 lightPosition;
layout (location = 18) uniform vec3 lightAmbient;
layout (location = 19) uniform vec3 lightDiffuse;
layout (location = 20) uniform vec3 lightSpecular;

layout (location = 13) uniform vec3 materialAmbient;
layout (location = 14) uniform vec3 materialDiffuse;
layout (location = 15) uniform vec3 materialSpecular;
layout (location = 16) uniform float shininess;

layout (location = 22) uniform samplerCube texSampler;
float etaX = 1/2.217;
float etaY = 1/2.417;
float etaZ = 1/2.617;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec4 fColor;
 
void main()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(vec3(lightPosition) - position);
	vec3 viewDir = normalize(-position);

	vec3 ambient = lightAmbient * materialAmbient;
	vec3 diffuse = lightDiffuse * materialDiffuse * max(dot(lightDir, norm), 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);

	vec3 refl = reflect(vec3(0.0, 0.0, 0.0) - lightDir, norm);
	specular = pow(max(0.0,  dot(norm, normalize(lightDir + viewDir))), shininess ) * materialSpecular * lightSpecular;

	vec3 incident_eye = normalize(position);
	vec3 reflected = reflect(incident_eye, norm);
	vec3 refractedX = refract(incident_eye, norm, etaX);
	vec3 refractedY = refract(incident_eye, norm, etaY);
	vec3 refractedZ = refract(incident_eye, norm, etaZ);
	float r = max(0.0, min(1.0, 4.0 * pow(1.0 + dot(incident_eye, norm), 1.0)));
	vec4 refractedColor = vec4(texture(texSampler, refractedX).x, texture(texSampler, refractedY).y, texture(texSampler, refractedZ).z, 1.0);
	vec4 color = r * texture(texSampler, reflected) + (1 - r) * refractedColor + vec4(specular, 0.0) + vec4(ambient,0.0);
	fColor = mix(color, vec4(0, 0, 0, 0), length(position) / 50); 

}
#version 330
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 17) uniform vec4 lightPosition;
layout (location = 18) uniform vec3 lightAmbient;
layout (location = 19) uniform vec3 lightDiffuse;
layout (location = 20) uniform vec3 lightSpecular;

layout(location = 3) uniform mat4 modelMatrix;
layout(location = 4) uniform mat4 viewMatrix;
layout(location = 5) uniform mat3 normalMatrix;
layout(location = 6) uniform mat4 projectionMatrix;
 
layout (location = 0) in vec4 vPosition; // pozycja wierzcholka w ukladzie modelu
layout (location = 1) in vec3 vNormal; // normal
layout (location = 2) in vec2 vTexCoord; // wspolrzedne tekstury
layout (location = 7) in vec3 vTangent;
layout (location = 8) in vec3 vBinormal;

out vec2 texCoords; // wspolrzedne tesktury
out vec3 lightDir;
out vec3 viewDir;
out vec3 position;

void main()
{
	texCoords = vTexCoord;
	
	position = vec3(viewMatrix * modelMatrix * vPosition);
	vec3 lightD = normalize( vec3( lightPosition ) - position );
	vec3 viewD = normalize( - position ); 

	vec3 tangent = normalize(normalMatrix * vTangent);
	vec3 normal = normalize(normalMatrix * vNormal);
	
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 binormal = cross(normal, tangent);

	mat3 M = transpose(mat3(tangent, binormal, normal)); // macierz przejscia do ukladu stycznego

	lightDir = normalize(M * lightD);
	viewDir = normalize(M * viewD);

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vPosition;
}
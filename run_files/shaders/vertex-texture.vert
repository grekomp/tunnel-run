#version 140
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

struct Light
{
	vec4 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light; // swiatlo 

layout(location = 3) uniform mat4 modelMatrix; // macierz modelu
layout(location = 4) uniform mat4 viewMatrix; // macierz widoku
layout(location = 5) uniform mat3 normalMatrix; // macierz do transformacji normala
layout(location = 6) uniform mat4 projectionMatrix; // macierz projekcji
 
layout (location = 0) in vec4 vPosition; // pozycja wierzcholka w ukladzie modelu
layout (location = 1) in vec3 vNormal; // normal
layout (location = 2) in vec2 vTexCoord; // wspolrzedne tekstury
layout (location = 7) in vec3 vTangent;
layout (location = 8) in vec3 vBinormal;

out vec2 texCoords; // wspolrzedne tesktury
out vec3 lightDir;
out vec3 viewDir;
 
void main()
{
	texCoords = vTexCoord;
	
	vec3 position = vec3(viewMatrix * modelMatrix * vPosition);
	vec3 lightD = normalize( vec3( light.position ) - position );
	vec3 viewD = normalize( vec3( 0.0, 0.0, 8.0 ) - position ); 

	vec3 tangent = normalize(normalMatrix * vTangent);
	vec3 binormal = normalize(normalMatrix * vBinormal);
	vec3 normal = normalize(normalMatrix * vNormal);

	mat3 M = transpose(mat3(tangent, binormal, normal)); // macierz przejscia do ukladu stycznego

	lightDir = normalize(M * lightD);
	viewDir = normalize(M * viewD);

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vPosition;
}
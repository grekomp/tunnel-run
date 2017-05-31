#version 330
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 3) uniform mat4 modelMatrix;
layout(location = 4) uniform mat4 viewMatrix;
layout(location = 5) uniform mat3 normalMatrix;
layout(location = 6) uniform mat4 projectionMatrix;
 
layout (location = 0) in vec4 vPosition; // pozycja wierzcholka w ukladzie modelu
layout (location = 1) in vec3 vNormal; // normal
layout (location = 2) in vec2 vTexCoord; // wspolrzedne tekstury

out vec3 position; // pozycja wierzcholka w ukladzie swiata
out vec3 normal; // normal w ukladzie swiata
out vec2 texCoord; // wspolrzedne tesktury
 
void main()
{
	position = vec3( viewMatrix * modelMatrix * vPosition);
	normal = normalize( normalMatrix * vNormal );
	texCoord = vTexCoord;
	
	gl_Position = projectionMatrix *  viewMatrix * modelMatrix * vPosition;
}
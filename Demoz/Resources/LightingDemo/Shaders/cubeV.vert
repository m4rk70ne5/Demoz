// vertex shader
#version 430

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 vTexCoord; //input texture coordinates
layout(location = 2) in vec4 normal;

uniform mat4 proj;
uniform mat4 camera;
uniform mat4 world;

smooth out vec4 vertexColor;
out vec2 gTexCoord;

// lighting
out vec3 FragPos;
out vec3 v_normal;

void main()
{
	// lighting
	//v_normal = vec3(normal);
	// transform the normal
	vec4 tfmNormal = vec4(vec3(normal), 0.0f);
	tfmNormal = world * tfmNormal;
	v_normal = vec3(tfmNormal);
	// the below statement could also work
	//v_normal = mat3(transpose(inverse(world))) * vec3(normal);
	FragPos = vec3(world * pos);
	
	// texture
	gTexCoord = vTexCoord;
	
	// final calculations
	gl_Position = proj * camera * world * pos;
}
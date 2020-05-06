#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 mvp;
uniform mat4 transformation;
uniform float time;

out vec3 interpolatedNormal;
out vec2 st;
out vec3 light;

void main() {
	gl_Position = mvp * transformation * vec4(position, 1.0);
	light = mat3(mvp) * vec3(0.0, 1.0, 1.0);    
	interpolatedNormal = mat3(mvp) * normal;
	st = texCoord;
}

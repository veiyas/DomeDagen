#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 mvp;
uniform mat4 transformation;
uniform mat4 view;
uniform mat3 normalMatrix;
uniform float time;

out vec3 fragPos;
out vec3 interpolatedNormal;
out vec2 st;
out vec3 light;
out vec3 view;


void main() {
	fragPos = vec3(transformation * vec4(position, 1.0));
    interpolatedNormal = normalMatrix * normal;
	st = texCoord;
	gl_Position = mvp * vec4(fragPos, 1.0);
}

#version 330 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertColor;

uniform mat4 mvp;
uniform mat3 transformation;

out vec3 fragColor;

void main() {
	fragColor = vertColor;
	gl_Position = mvp * vec4(transformation * vertPosition, 1.f);	
}
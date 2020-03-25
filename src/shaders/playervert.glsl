#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 MV;
uniform mat4 P;
uniform float time;

out vec3 interpolatedNormal;
out vec2 st;
out vec3 light;

void main(){
    light = mat3(MV) * vec3(0.0, 1.0, 1.0);
    gl_Position = (P * MV) * vec4(position, 1.0);
    interpolatedNormal = mat3(MV) * normal;
    st = texCoord;
}

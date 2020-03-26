#version 330 core

uniform float time;
uniform sampler2D tex;
in vec2 st;

in vec3 interpolatedNormal;

in vec3 light;

out vec4 color;

void main() {
     vec4 texcolor = texture(tex, st);
     vec3 nNormal = normalize(interpolatedNormal);
     float diffuse = max(0.0, dot(nNormal, light));

     //Only ambient light for now
     color = texcolor;

//     color = vec4(1.0, 1.0, 1.0, 1.0);
}
#version 330 core

uniform float time;
uniform sampler2D tex;
in vec2 st;

in vec3 interpolatedNormal;

in vec3 light;
in vec3 view;

out vec4 color;

//in VS_OUT{
 //   vec3 normal;
 //   vec3 view;
//} fs_in;

void main() {
     vec4 texcolor = texture(tex, st);
     vec3 nNormal = normalize(interpolatedNormal);
     float diffuse = max(0.0, dot(nNormal, light));

     //Only ambient light for now
     color = texcolor;

//     color = vec4(1.0, 1.0, 1.0, 1.0);


     vec3 u = normalize(view); 
     vec3 r = reflect(u, normalize(interpolatedNormal));
     r.z += 1.0;
     float m = 0.5*inversesqrt(dot(r, r));
     //color = texture(tex, r.xy*m + vec2(0.5));
}
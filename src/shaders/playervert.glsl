#version 330 core
layout(location = 0) in vec3  position;
layout(location = 1) in vec3  normal;
layout(location = 2) in vec2  texCoord;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4  weights;

const int MAX_BONES = 100; // ????

uniform mat4  mvp;
uniform mat4  transformation;
uniform mat4  view;
uniform mat3  normalMatrix;
uniform float time;
uniform mat4  bones[MAX_BONES];

out vec3 fragPos;
out vec3 interpolatedNormal;
out vec2 st;
out vec3 light;

void main() {
    mat4 boneTransform = bones[boneIds[0]] * weights[0];
    boneTransform     += bones[boneIds[1]] * weights[1];
    boneTransform     += bones[boneIds[2]] * weights[2];
    boneTransform     += bones[boneIds[3]] * weights[3];

    /*vec4 PosL = boneTransform * vec4(position, 1.0);
    gl_Position = mvp * transformation * PosL;
    TexCoord0 = TexCoord;
    vec4 NormalL = BoneTransform * vec4(Normal, 0.0);
    interpolatedNormal = (gWorld * normalMatrix).xyz;
    fragPos = (gWorld * PosL).xyz;*/

	/*fragPos = vec3(transformation * vec4(position, 1.0));
    interpolatedNormal = normalMatrix * normal;
	st = texCoord;
	gl_Position = mvp * vec4(fragPos, 1.0);*/

    vec4 bonePos = boneTransform * vec4(position, 1.0);
    fragPos = vec3(transformation * bonePos);
    vec4 boneNormal = boneTransform * vec4(normal, 0.0);
    interpolatedNormal = (mat4(normalMatrix) * boneNormal).xyz;
	st = texCoord;
	gl_Position = mvp * vec4(fragPos, 1.0);
}

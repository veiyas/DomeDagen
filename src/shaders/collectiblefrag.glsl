#version 330 core

uniform float time;
uniform sampler2D tex;
uniform vec3 cameraPos;

in vec2 st;
in vec3 interpolatedNormal;
in vec3 fragPos;

out vec4 color;

// Preset shader parameters. Can be modified from the game logic as well.
uniform float diffuseStrength = 0.25;
uniform float ambientStrength = 0.75;
uniform float shininess = 15;
uniform float specularStrength = 0.6;
uniform vec4 specularColour = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3 lightPos = vec3(0.0, -100.0, 100.0);

void main() {
	vec3 lightDir = normalize(lightPos - fragPos);

	float tex = vec4(texture(tex, st)).r;

	// Diffuse light
	vec3 nNormal = normalize(interpolatedNormal);
	float diffuseAmount = max(0.0, dot(nNormal, lightDir));

	// Specular light (only on masked areas)
	vec3 rLight = normalize(2 * dot(nNormal, lightDir) * nNormal - lightDir);
	vec3 viewDir = normalize(cameraPos - fragPos);
	float specularAmount = pow(max(0.0, dot(rLight, viewDir)), shininess);

	color = tex * (ambientStrength + diffuseAmount * diffuseStrength)
		+ specularAmount * specularStrength * specularColour;
}

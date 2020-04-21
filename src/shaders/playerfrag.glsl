#version 330 core

uniform float time;
uniform sampler2D tex;
uniform vec3 primaryCol;
uniform vec3 secondaryCol;

in vec2 st;
in vec3 interpolatedNormal;
in vec3 light;

out vec4 color;

// Preset shader parameters. Can be modified from the game logic as well.
uniform float diffuseStrength = 0.25;
uniform vec4 diffuseColour = vec4(1.0, 1.0, 1.0, 1.0);
uniform float ambientStrength = 0.75;
uniform float shininess = 25;
uniform float specularStrength = 0.8;
uniform vec4 specularColour = vec4(1.0, 1.0, 1.0, 1.0);

void main() {
    // Normalized normal, light reflection and "view" vectors
    vec3 nNormal = normalize(interpolatedNormal);
    vec3 rLight = normalize(2 * dot(nNormal, light) * nNormal - light);
    vec3 view = vec3(0.0, 0.0, 1.0); // This feels wrong and also probably is wrong :')

    // Albedo colour
    float mask = vec4(texture(tex, st)).r;
    vec4 albedo = (1.0 - mask) * vec4(primaryCol, 1.0) + mask * vec4(secondaryCol, 1.0);

    // Diffuse light
    float diffuseAmount = max(0.0, dot(nNormal, light));

    // Specular light (only on masked areas)
    float specularAmount = mask * pow(max(0.0, dot(rLight, view)), shininess);
    
    color = albedo * ambientStrength + diffuseAmount * diffuseStrength * diffuseColour + specularAmount * specularStrength * specularColour;
}
#version 330 core

in vec3 fragmentColor;
in vec3 normalVec;

out vec4 screenColor;

const vec3 ambientColor = vec3(0.1,0.1,0.1);

const vec3 DirectionalLightColor = vec3(1.0,1.0,1.0);
const vec3 DirectionalLightDir = vec3(1.0,1.0,-1.0);

void main()
{
    vec3 lightDir = normalize(DirectionalLightDir);

	
	float lightIntensity = max(dot(normalVec, lightDir), 0.0f);

	vec3 light = ambientColor + (lightIntensity * DirectionalLightColor);
    screenColor = vec4(light*fragmentColor, 1.0);
}
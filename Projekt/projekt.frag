#version 150

out vec4 outColor;
in vec2 texCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

in vec3 exNormal;

in vec3 fragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ghostIntensity;
// attenuation constants
const float lightConstant = 1.0;
const float lightLinear = 0.09;
const float lightQuadratic = 0.032;

void main(void)
{
	// Get texture pixel
	vec3 texColor = texture(tex1, texCoord).rgb;

	// Normal (ensure consistent orientation)
	vec3 N = normalize(exNormal);

	// Light vector and attenuation
	vec3 L = lightPos - fragPos;
	float distance = length(L);
	L = normalize(L);

	float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));

	// Diffuse term
	float diff = max(dot(N, L), 0.0);

	// Ambient
	float ambientStrength = 0.12;

	vec3 lighting = (ambientStrength * texColor) + (texColor * diff * lightColor * attenuation);

	vec3 ghostTint = vec3(1, 0, 0);
	vec3 finalColor = mix(lighting, ghostTint, ghostIntensity * 0.25);
	outColor = vec4(finalColor, 1.0);
}

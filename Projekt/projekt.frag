#version 150

in vec2 texCoord;
in vec3 exNormal;

out vec4 outColor;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main(void)
{
	// Get texture pixel
	vec3 texColor = vec3(0, 0, 0);
	texColor = texture(tex1, texCoord).rgb;

	// Fake lighting
	vec3 N = normalize(-exNormal);
	vec3 L = normalize(vec3(-0.3, 0.8, -0.2)); // Source
    float diff = max(dot(N, L), 0.0); // Diffuse lighting

	// Combine texture, lighting + ambient
	float ambient = 0.2;
	vec3 finalColor = texColor * (ambient + diff);
    
    outColor = vec4(finalColor, 1.0);
}

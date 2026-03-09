#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;

in vec3 exNormal;

void main(void)
{
	vec3 N = normalize(-exNormal); // Our normals were flipped in the wrong direction?
	vec3 L = normalize(vec3(-0.3, 0.8, -0.2)); // lightsource
    float diff = max(dot(N, L), 0.0); // Diffuse lighting

    vec3 texColor = texture(tex, texCoord).rgb;
	
	float ambient = 0.2;
	vec3 finalColor = texColor * (ambient + diff);
    
    outColor = vec4(finalColor, 1.0);
}

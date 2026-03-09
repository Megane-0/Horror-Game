#version 150

out vec4 outColor;
in vec2 texCoord;
in vec3 pixelPos;

uniform sampler2D tex1;
uniform sampler2D tex2;

in vec3 exNormal;

void main(void)
{
	vec3 N = normalize(-exNormal); // Our normals were flipped in the wrong direction?
	vec3 L = normalize(vec3(-0.3, 0.8, -0.2)); // lightsource
    float diff = max(dot(N, L), 0.0); // Diffuse lighting



	/*
	vec3 texColor = vec3(0, 0, 0);
	if (pixelPos.y > 3) {
		texColor = texture(tex1, texCoord).rgb;
	} else {
		texColor = texture(tex2, texCoord).rgb;
	}*/


	/*
    vec3 texColor = sin(pixelPos.z*10.0) * texture(tex1,
		texCoord.st) + (1.0 - sin(pixelPos.z*10.0)) *
		texture(tex2, texCoord.st);//texture(tex1, texCoord).rgb;
	*/

	/*
	float ambient = 0.2;
	vec3 finalColor = texColor * (ambient + diff);
    
    outColor = vec4(finalColor, 1.0);
	*/

	vec4 checker = texture(tex1, texCoord.st);
	vec4 dandylion = texture(tex2, texCoord.st);

	outColor = sin(pixelPos.x*5.0) * dandylion +
				(1.0 - sin(pixelPos.x*5.0)) * checker;
}

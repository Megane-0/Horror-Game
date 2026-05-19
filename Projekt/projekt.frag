#version 150

in vec2 texCoord;
in vec3 exNormal;
in vec3 surf;

out vec4 outColor;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform vec3 cameraPos;
uniform vec3 cameraDir;
uniform float flashlightCos;

const float lightStr = 100;






// Slightly broken
float getSpecular(vec3 lightDirection, vec3 normal, vec3 eyeDirection) {
	vec3 reflectedLightDirection = reflect(-lightDirection, exNormal);

	float specularStrength = 0.0;

	if (dot(lightDirection, exNormal) > 0.0)
	{
	specularStrength = dot(reflectedLightDirection, eyeDirection);
	float exponent = 200.0;
	specularStrength = max(specularStrength, 0);
	specularStrength = pow(specularStrength, exponent);
	}

	return specularStrength;
}


void main(void)
{
	// Get texture pixel
	vec3 texColor = vec3(0, 0, 0);
	texColor = texture(tex1, texCoord).rgb;

	// Phong diffuse
	vec3 light = normalize(-cameraDir);
	float diff = clamp(dot(normalize(exNormal), light), 0, 1);

	// Phong specular
	vec3 viewDir = normalize(cameraPos - surf);
	vec3 norm = normalize(exNormal);
	float spec = getSpecular(light, viewDir, norm);

	// Flashlight cone check: angle between (camera→surface) and camera look direction
	float ambient = 0.2;	
	vec3 finalColor;

	float cosAngle = dot(normalize(cameraPos - surf), light);

	if (cosAngle > flashlightCos) {
		finalColor = texColor * ((diff+spec)*(cosAngle-flashlightCos)*lightStr + ambient); 
		//finalColor = texColor * (spec+ambient+diff); // For testing
	} else {
		finalColor = texColor * ambient;
	}
    
	// Finally, add the color
    outColor = vec4(finalColor, 1.0);
}



/*
TO DO:
- pass the character position + dir to the shader
- choose the character dir as new light source
- add cosine constant to project and pass to shader
- calculate flashlight cutoff

- add flashlight smooth cutoff
- add light attenuation
- Add specular shading part to phong
- add button to turn off light
*/
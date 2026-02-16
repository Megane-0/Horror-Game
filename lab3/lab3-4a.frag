#version 150

uniform sampler2D texUnit;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent;
uniform bool isDirectional[4];


in vec3 exNormal;
in vec2 outTexCoord;
in vec3 surf;
out vec4 out_Color;


void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58);
	float shade; 
	shade = dot(normalize(exNormal), light);
	shade = clamp(shade, 0, 1);


	vec3 reflectedLightDirection = reflect(-lightSourcesDirPosArr[3], exNormal);
	vec3 eyeDirection = normalize(-surf);

	float specularStrength = 0.0;
	if (dot(lightSourcesDirPosArr[3], exNormal) > 0.0){
		specularStrength = dot(reflectedLightDirection, eyeDirection);
		specularStrength = max(specularStrength, 0.01);
		specularStrength = pow(specularStrength, specularExponent);
	}

	out_Color = vec4(specularStrength*0.5 + 0.5);
}

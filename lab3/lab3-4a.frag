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
	
	float shade, diffuse, specular; 
	
	// Diffuse
	
	diffuse = dot(normalize(exNormal), light);
	diffuse = max(0.0, diffuse); // No negative light

	// Specular
	vec3 reflectedLightDirection = reflect(-lightSourcesDirPosArr[3], normalize(exNormal));
	
	vec3 eyeDirection = normalize(-surf);

	specular = dot(reflectedLightDirection, eyeDirection);
	if (specular > 0.0){
		specular = 1.0 * pow(specular, specularExponent);
	}
	specular = max(specular, 0.0);
	shade = 0.8*diffuse + 0.8*specular;

	out_Color = vec4(shade, shade, shade, 1.0);
}

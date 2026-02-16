#version 150

uniform sampler2D texUnit;


in vec3 exNormal;
in vec2 outTexCoord;
out vec4 out_Color;


void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58);
	float shade; 
	shade = dot(normalize(exNormal), light);
	shade = clamp(shade, 0, 1);

	//out_Color = vec4(shade, shade, shade, 1.0);
	out_Color = texture(texUnit, outTexCoord);


}

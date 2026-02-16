#version 150

uniform sampler2D texUnit;


in vec3 exNormal;
in vec2 outTexCoord;
out vec4 out_Color;


void main(void)
{	
	out_Color = texture(texUnit, outTexCoord);

}

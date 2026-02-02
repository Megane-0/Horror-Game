#version 150

uniform sampler2D texUnit;

in vec3 v_Color;
in vec2 outTexCoord;
out vec4 out_Color;

void main(void)
{
	//out_Color = texture(texUnit, outTexCoord);
	out_Color = vec4(v_Color, 1.0); 
}

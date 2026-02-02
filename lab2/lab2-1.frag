#version 150

in vec4 v_Color;
in vec2 outTexCoord;
out vec4 out_Color;

void main(void)
{
	out_Color = vec4(outTexCoord.sstt);
}

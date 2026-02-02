#version 150

uniform sampler2D texUnit;

in vec4 v_Color;
in vec2 outTexCoord;
out vec4 out_Color;

void main(void)
{
	out_Color = texture(texUnit, outTexCoord);
}

#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;

in vec3 exNormal;

void main(void)
{
	//outColor = texture(tex, texCoord);
	outColor = vec4(exNormal, 1.0);
}

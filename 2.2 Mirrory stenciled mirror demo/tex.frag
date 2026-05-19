#version 150

// Simple texture shader
// Texture coordinates from vertex shader

in vec2 texcoord;
out vec4 outColor;
uniform sampler2D tex;
uniform float blend;

void main(void)
{
	outColor = texture(tex, texcoord);
	outColor.a = blend;
}

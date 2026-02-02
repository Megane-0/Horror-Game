#version 150
uniform sampler2D texUnit;
uniform mat4 myMatrix;
uniform mat4 projectionMatrix;

in vec3 in_Position;
in vec3 in_Normal; 
in vec2 inTexCoord;

out vec4 v_Color;
out vec2 outTexCoord;

void main(void)
{
	gl_Position = projectionMatrix*myMatrix*vec4(in_Position, 1.0);
	v_Color = vec4(in_Normal.zzz, 1.0);
	outTexCoord = inTexCoord;
}

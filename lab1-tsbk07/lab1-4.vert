#version 150

in  vec3 in_Position;

uniform mat4 myMatrix;
in vec4 in_Color;
out vec4 v_Color;

void main(void)
{
	gl_Position = myMatrix * vec4(in_Position, 1.0);
	v_Color = in_Color;
}

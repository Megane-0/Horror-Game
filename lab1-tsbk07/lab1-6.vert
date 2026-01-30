#version 150

uniform mat4 myMatrix;
in vec3 in_Position;
in vec3 in_Normal; 
out vec4 v_Color;

void main(void)
{
	gl_Position = myMatrix * vec4(in_Position, 1.0);
	v_Color = vec4(in_Normal.zzz, 1.0);
}

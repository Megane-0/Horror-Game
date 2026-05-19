#version 150

// Simple example with transformation matrices and passing on texture coordinates

in  vec3 in_Position;
in  vec2 in_Texcoord;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
out  vec2 texcoord;

void main(void)
{
	gl_Position = projectionMatrix * modelViewMatrix * vec4(in_Position, 1.0);
	texcoord = in_Texcoord;
}

#version 150

uniform mat4 projectionMatrix;
uniform mat4 lookAtMatrix;
uniform mat4 myMatrix;
uniform mat4 cameraMovement;



in vec3 in_Position;
in vec3 in_Normal; 
in vec2 in_Texture;


out vec3 exNormal;
out vec2 outTexCoord;

void main(void)
{


	gl_Position = projectionMatrix*cameraMovement*lookAtMatrix*myMatrix*vec4(in_Position, 1.0);

	exNormal = in_Normal;
	outTexCoord = in_Texture;
}

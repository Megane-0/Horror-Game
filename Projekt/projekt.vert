#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;
out vec3 exNormal;

uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 placeDowns;

void main(void)
{
	// Pass texture and normal to frag
	texCoord = inTexCoord;
	exNormal = inNormal;

	// Calculate transformations
	gl_Position = projMatrix * viewMatrix * modelMatrix * placeDowns *vec4(inPosition, 1.0);
	
	//surf = inPosition; 
}

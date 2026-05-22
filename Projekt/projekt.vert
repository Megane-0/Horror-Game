#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;
out vec3 exNormal;
out vec3 surf;
out vec3 fragPos;

uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 placeDowns;

void main(void)
{
	// Pass texture and normal to frag
	texCoord = inTexCoord;


	exNormal = mat3(modelMatrix * placeDowns) * inNormal;

	mat4 model = modelMatrix*placeDowns;

	// World-space fragment position
	fragPos = vec3(model * vec4(inPosition, 1.0));

	// Calculate transformations
	gl_Position = projMatrix * viewMatrix * modelMatrix * placeDowns *vec4(inPosition, 1.0);
	
	surf = vec3(modelMatrix * placeDowns * vec4(inPosition, 1.0));
}

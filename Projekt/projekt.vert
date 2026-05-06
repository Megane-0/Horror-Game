#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;
out vec3 exNormal;
out vec3 fragPos;

uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 placeDowns;


void main(void)
{
	// Pass texture to frag
	texCoord = inTexCoord;

	// Normal calculations (include placeDowns in transform)
	mat4 model = modelMatrix * placeDowns;
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	exNormal = normalMatrix * inNormal;

	// World-space fragment position
	fragPos = vec3(model * vec4(inPosition, 1.0));

	// Calculate transformations
	gl_Position = projMatrix * viewMatrix * model * vec4(inPosition, 1.0);
}

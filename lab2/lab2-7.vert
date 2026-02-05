#version 150



uniform sampler2D texUnit;
uniform mat4 myMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lookAtMatrix;

in vec3 in_Position;
in vec3 in_Normal; 
in vec2 in_Texture;

out vec3 v_Color;
out vec2 outTexCoord;
out vec3 exNormal;

void main(void)
{
	mat3 normalMatrix1 = mat3(myMatrix);

	gl_Position = projectionMatrix*lookAtMatrix*myMatrix*vec4(in_Position, 1.0);
	outTexCoord = in_Texture;
	vec3 transformedNormal = normalMatrix1 * in_Normal;

	exNormal = transformedNormal;
}

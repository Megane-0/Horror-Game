#version 150



uniform sampler2D texUnit;
uniform mat4 myMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lookAtMatrix;

in vec3 in_Position;
in vec3 in_Normal; 
in vec2 inTexCoord;

out vec3 v_Color;
out vec2 outTexCoord;

void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58);
	mat3 normalMatrix1 = mat3(myMatrix);
	float shade;


	gl_Position = projectionMatrix*lookAtMatrix*myMatrix*vec4(in_Position, 1.0);
	outTexCoord = inTexCoord;
	vec3 transformedNormal = normalMatrix1 * in_Normal;

	shade = dot(normalize(transformedNormal), light);
	shade = max(shade, 0);
	v_Color = vec3(shade);
}

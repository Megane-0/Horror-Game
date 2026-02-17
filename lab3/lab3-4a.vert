#version 150

uniform mat4 projectionMatrix;
uniform mat4 lookAtMatrix;
uniform mat4 myMatrix;
uniform mat4 cameraMovement;




in vec3 in_Position;
in vec3 in_Normal; 
in vec2 in_Texture;


out vec3 exNormal;
out vec3 surf;
void main(void)
{

	mat4 modelView = cameraMovement*lookAtMatrix*myMatrix;

	vec4 posView = modelView*vec4(in_Position, 1.0);

	gl_Position = projectionMatrix*posView;

	// surface position in veiw space
	surf = posView.xyz;

	mat3 normalMatrix = transpose(inverse(mat3(modelView)));
	exNormal = normalMatrix*in_Normal;
	/*
	gl_Position = projectionMatrix*cameraMovement*lookAtMatrix*myMatrix*vec4(in_Position, 1.0);
	surf = in_Position; // for specular
	exNormal = in_Normal;
	outTexCoord = in_Texture;*/
}

#version 150

uniform sampler2D texUnit;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent;
uniform bool isDirectional[4];
uniform mat4 lookAtMatrix;
uniform mat4 cameraMovement;


in vec3 exNormal;
in vec3 surf;

out vec4 out_Color;


void main(void)
{
	vec3 N = normalize(exNormal);
	vec3 V = normalize(-surf); // camera (0,0,0) in view space

	vec3 totalLight = vec3(0.0);

	for(int i = 0; i < 4; i++) // Loop through lightsources
    {
        vec3 L;

        if(isDirectional[i]) // if directional
        {
            // Directional light
            L = normalize((cameraMovement * lookAtMatrix * vec4(lightSourcesDirPosArr[i], 0.0)).xyz);
        }
        else 
        {
            // Positional light
            vec3 lightPosView = (cameraMovement * lookAtMatrix * vec4(lightSourcesDirPosArr[i], 1.0)).xyz;
			L = normalize(lightPosView - surf);
        }

        // Diffuse
        float diff = max(dot(N, L), 0.0);

        // Specular
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(R, V), 0.0), specularExponent);

        totalLight += lightSourcesColorArr[i] * (0.5 * diff + 0.8 * spec);
    }
	out_Color = vec4(totalLight, 1.0);

}

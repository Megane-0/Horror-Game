#version 150
in vec2 vUV;
out vec4 outColor;

uniform vec3 crossColor;
uniform sampler2D uiTex;
uniform int useTexture;

void main()
{
    if (useTexture == 1)
        outColor = texture(uiTex, vUV);
    else
        outColor = vec4(crossColor, 1.0);
}
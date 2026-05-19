#version 150

in vec2 vUV;
out vec4 outColor;

uniform sampler2D sceneTex;
uniform float vignetteStrength; // 0..1
uniform float ghostIntensity; // 0..1


void main()
{
    vec3 color = texture(sceneTex, vUV).rgb;

    // enkel vignette
    vec2 centered = vUV * 2.0 - 1.0;
    float v = smoothstep(1.0, 0.2, length(centered));
    color *= mix(1.0, v, vignetteStrength);

    // ghost tint
    color = mix(color, vec3(0.5, 0.1, 0.7), ghostIntensity * 0.1);
    //outColor = vec4(color, 1.0);
    outColor = texture(sceneTex, vUV);
}
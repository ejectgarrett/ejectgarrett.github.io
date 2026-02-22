#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D textureSampler;
uniform float time;
uniform vec2 resolution;
uniform float squareSize;

out vec4 finalColor;

void main()
{
    vec2 totalTiles = resolution / squareSize;

    vec2 tiledUV = fract(fragTexCoord * totalTiles + vec2(time * 0.5, 0.0));

    vec4 texel = texture(textureSampler, tiledUV);
    
    finalColor = texel * fragColor;
}
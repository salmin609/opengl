#version 330 core
out vec4 fragColor;

in vec4 Vertex_UV;

uniform sampler2D screenTextures;

vec4 Normal()
{
	return texture(screenTextures, Vertex_UV.xy);
}
vec4 Inversion()
{
	return vec4(vec3(1.0 - texture(screenTextures, Vertex_UV.xy)), 1.0);
}
vec4 GrayScale()
{
	vec4 color = texture(screenTextures, Vertex_UV.xy);
	float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	return vec4(average, average, average, 1.0);
}
const float offset = 1.0 / 300.0;

vec4 Kernal()
{
	vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // ÁÂÃø »ó´Ü
        vec2( 0.0f,    offset), // Áß¾Ó »ó´Ü
        vec2( offset,  offset), // ¿ìÃø »ó´Ü
        vec2(-offset,  0.0f),   // ÁÂÃø Áß¾Ó
        vec2( 0.0f,    0.0f),   // Á¤Áß¾Ó
        vec2( offset,  0.0f),   // ¿ìÃø Áß¾Ó
        vec2(-offset, -offset), // ÁÂÃø ÇÏ´Ü
        vec2( 0.0f,   -offset), // Áß¾Ó ÇÏ´Ü
        vec2( offset, -offset)  // ¿ìÃø ÇÏ´Ü   
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTextures, Vertex_UV.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    return vec4(col, 1.0);
}

vec4 Blur()
{
    vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // ÁÂÃø »ó´Ü
            vec2( 0.0f,    offset), // Áß¾Ó »ó´Ü
            vec2( offset,  offset), // ¿ìÃø »ó´Ü
            vec2(-offset,  0.0f),   // ÁÂÃø Áß¾Ó
            vec2( 0.0f,    0.0f),   // Á¤Áß¾Ó
            vec2( offset,  0.0f),   // ¿ìÃø Áß¾Ó
            vec2(-offset, -offset), // ÁÂÃø ÇÏ´Ü
            vec2( 0.0f,   -offset), // Áß¾Ó ÇÏ´Ü
            vec2( offset, -offset)  // ¿ìÃø ÇÏ´Ü   
        );

        float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
        );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(screenTextures, Vertex_UV.st + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];

        return vec4(col, 1.0);
}


void main()
{
	fragColor = Normal();
}
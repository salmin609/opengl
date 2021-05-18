#version 430

in vec2 ex_TexCoor;
in vec3 fsInColor;
out vec4 colorOut;

uniform sampler2D texture;

void main(void){
    vec4 outputColor = texture2D(texture, ex_TexCoor) * 3.f;

    float rgbLength = length(outputColor.rgb);
    if(rgbLength < 0.3f)
        outputColor.w = 0.0f;

    
    outputColor.x *= (fsInColor.x );
    outputColor.y *= (fsInColor.y );
    outputColor.z *= (fsInColor.z );


    colorOut = outputColor;
}
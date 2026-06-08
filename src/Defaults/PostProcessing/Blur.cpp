//
// Created by joost on 5/29/26.
//

#include "Defaults/PostProcessing/Blur.h"

constexpr const char* blurSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uInputTexture0;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(uInputTexture0, 0); // gets size of single texel
    vec3 result = texture(uInputTexture0, TexCoords).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uInputTexture0, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(uInputTexture0, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uInputTexture0, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(uInputTexture0, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
)glsl";

void Blur::setup() {
    PostProcessingEffect blur = PostProcessingEffect::fromSource(blurSource, 1, 1);

    for (int i = 0; i < 5; i++) {
        blur.addBoolUniform("horizontal", false);
        blur.commitPass();
        blur.addBoolUniform("horizontal", true);
        blur.commitPass();
    }
    addEffect(std::move(blur));
}

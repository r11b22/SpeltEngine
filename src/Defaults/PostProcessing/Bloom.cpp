//
// Created by joost on 5/29/26.
//


#include "Defaults/PostProcessing/Bloom.h"


constexpr const char* splitSource = R"glsl(
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D uInputTexture0;

void main()
{
    vec4 outColor = texture(uInputTexture0, TexCoords);
    FragColor = outColor;

    float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(outColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)glsl";

constexpr const char* blurSource = R"glsl(
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Blurred;

in vec2 TexCoords;

uniform sampler2D uInputTexture0;
uniform sampler2D uInputTexture1;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(uInputTexture1, 0); // gets size of single texel
    vec3 result = texture(uInputTexture1, TexCoords).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uInputTexture1, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(uInputTexture1, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uInputTexture1, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(uInputTexture1, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    Blurred = vec4(result, 1.0);
    vec4 outColor = texture(uInputTexture0, TexCoords);
    FragColor = outColor;
}
)glsl";

constexpr const char* combineSource = R"glsl(
#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uInputTexture0;
uniform sampler2D uInputTexture1;
uniform float uExposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(uInputTexture0, TexCoords).rgb;
    vec3 bloomColor = texture(uInputTexture1, TexCoords).rgb;
    hdrColor += bloomColor; // additive blending

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * uExposure);

    // gamma correct
    //result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}
)glsl";

void Bloom::setup() {
    PostProcessingEffect split = PostProcessingEffect::fromSource(splitSource, 1, 2);
    split.addBoolUniform("horizontal", false);
    split.commitPass();
    addEffect(std::move(split));

    PostProcessingEffect blur = PostProcessingEffect::fromSource(blurSource, 2, 2);
    for (int i = 0; i < 5; i++) {
        blur.addBoolUniform("horizontal", false);
        blur.commitPass();
        blur.addBoolUniform("horizontal", true);
        blur.commitPass();
    }
    addEffect(std::move(blur));

    PostProcessingEffect combine = PostProcessingEffect::fromSource(combineSource, 2, 1);
    combine.addFloatUniform("uExposure", 1.2f);
    combine.commitPass();
    addEffect(std::move(combine));
}

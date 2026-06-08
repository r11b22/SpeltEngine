//
// Created by joost on 5/29/26.
//

#ifndef CGVCPROJECT_SHADERSOURCE_H
#define CGVCPROJECT_SHADERSOURCE_H

constexpr const char* straightToScreenVertexShader =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNorm;\n" // Unused, but won't crash
"layout (location = 2) in vec2 aUV;\n"
"out vec2 TexCoords;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"    TexCoords = aUV;\n"
"}";

constexpr const char* straightToScreenFragmentShader = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, TexCoords);
}
)glsl";

constexpr const char* pickingFragment = R"glsl(
#version 330 core

uniform uint uObjectIndex;

layout (location = 0) out uint fragID;

void main()
{
    fragID = uObjectIndex;
}
)glsl";

constexpr const char* pickingVertex = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

void main()
{
    vec4 position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * position;
}
)glsl";


constexpr const char* standardVertexShader = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;

void main()
{
    FragPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    // vec4 position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * position;
    gl_Position = uProjectionMatrix * uViewMatrix * vec4(FragPos, 1.0);

    //Normal = normalize(aNorm);
    Normal = mat3(transpose(inverse(uModelMatrix))) * aNorm;
    UV = aUV;
}
)glsl";

constexpr const char* litFragmentShader = R"glsl(
#version 330 core
layout (location = 0) out vec4 FragColor;

#define MAX_POINT_LIGHTS 5

struct Light{
    vec3 position;
    vec3 diffuse;
    vec3 specular;

    // uitdoving
    float constant;
    float lineair;
    float quadratic;
};

uniform vec3 uAmbient;

uniform Light uPointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

uniform sampler2D uTexture;
uniform float uSpecularStrength;

uniform vec3 uCameraPos; // camera location
in vec3 FragPos; // pixel position
in vec3 Normal;
in vec2 UV;

void main()
{
    vec4 texColor = texture(uTexture, UV);

    if (texColor.a < 0.5){
        discard;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(uCameraPos - FragPos);
    vec3 totalLight = uAmbient*vec3(texColor);

    for (int i = 0; i < numPointLights; i++){
        Light light = uPointLights[i];

        vec3 lightDirection = normalize(light.position - FragPos);
        float distance = length(light.position - FragPos);

        float diff = max(dot(norm, lightDirection), 0.0);

        vec3 reflectDir = reflect(-lightDirection, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

        float attenuation = 1.0 / (light.constant + light.lineair * distance + light.quadratic * (distance * distance));
        vec3 diffuse = light.diffuse * diff * vec3(texColor);
        vec3 specular = light.specular * spec * uSpecularStrength;

        totalLight += ((diffuse + specular) * attenuation);
    }

    vec4 outColor = vec4(totalLight, texColor.a);
    FragColor = outColor;

    /*float brightness = dot(outColor.rgb, vec3(0.216, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(outColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);*/
}
)glsl";

#endif //CGVCPROJECT_SHADERSOURCE_H
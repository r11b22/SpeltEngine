//
// Created by joost on 5/29/26.
//

#pragma once

namespace Spelt {
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


    constexpr const char *standardVertexShader = R"glsl(
    #version 430 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNorm;
    layout (location = 2) in vec2 aUV;

    struct InstanceData {
        mat4 uModelMatrix;
    };

    layout(std430, binding = 3) readonly buffer InstanceBuffer {
        InstanceData instances[];
    };

    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;

    out vec3 FragPos;
    out vec3 Normal;
    out vec2 UV;

    void main()
    {
        mat4 modelMatrix = instances[gl_InstanceID].uModelMatrix;

        FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
        // vec4 position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        // gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * position;
        gl_Position = uProjectionMatrix * uViewMatrix * vec4(FragPos, 1.0);

        //Normal = normalize(aNorm);
        Normal = mat3(transpose(inverse(modelMatrix))) * aNorm;
        UV = aUV;
    }
    )glsl";

    constexpr const char *litFragmentShader = R"glsl(
    #version 430 core
    layout (location = 0) out vec4 FragColor;

    #define MAX_POINT_LIGHTS 64

    struct PointLight{
        vec3 position;
        vec3 diffuse;
        vec3 specular;

        // uitdoving
        float constant;
        float lineair;
        float quadratic;
    };
    uniform PointLight uPointLights[MAX_POINT_LIGHTS];
    uniform int uNumPointLights;

    #define MAX_DIRECTIONAL_LIGHTS 5

    struct DirectionalLight{
        vec3 direction;
        vec3 diffuse;
        vec3 specular;
    };

    uniform DirectionalLight uDirectionalLights[MAX_DIRECTIONAL_LIGHTS];
    uniform int uNumDirectionalLights;

    uniform vec3 uAmbient;

    struct MaterialProperties{
        int materialType;
        float diffuse;
        float specular;
        float shininess;
    };

    uniform MaterialProperties uMaterialProperties;


    uniform sampler2D uTexture;
    uniform vec3 uColor;

    uniform vec3 uCameraPos; // camera location
    in vec3 FragPos; // pixel position
    in vec3 Normal;
    in vec2 UV;

    void main()
    {
        vec4 texColor;

        switch (uMaterialProperties.materialType){
            case 0: // Colored
                texColor = vec4(uColor, 1.0f);
                break;
            case 1: // Textured
                texColor = texture(uTexture, UV);
                break;
        }


        if (texColor.a < 0.5){
            discard;
        }

        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(uCameraPos - FragPos);
        vec3 totalLight = uAmbient*vec3(texColor);

        for (int i = 0; i < uNumPointLights; i++){
            PointLight light = uPointLights[i];

            vec3 lightDirection = normalize(light.position - FragPos);
            float distance = length(light.position - FragPos);

            float diff = max(dot(norm, lightDirection), 0.0);

            vec3 reflectDir = reflect(-lightDirection, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterialProperties.shininess);

            float attenuation = 1.0 / (light.constant + light.lineair * distance + light.quadratic * (distance * distance));
            vec3 diffuse = uMaterialProperties.diffuse * light.diffuse * diff * vec3(texColor);
            vec3 specular = uMaterialProperties.specular * light.specular * spec;

            totalLight += ((diffuse + specular) * attenuation);
        }

        for (int i = 0; i < uNumDirectionalLights; i++){
            DirectionalLight light = uDirectionalLights[i];

            vec3 lightDirection = normalize(light.direction);

            float diff = max(dot(norm, lightDirection), 0.0);

            vec3 reflectDir = reflect(-lightDirection, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0),  uMaterialProperties.shininess);


            vec3 diffuse = uMaterialProperties.diffuse * light.diffuse * diff * vec3(texColor);
            vec3 specular = uMaterialProperties.specular * light.specular * spec;

            totalLight += (diffuse + specular);
        }

        vec4 outColor = vec4(totalLight, texColor.a);
        FragColor = outColor;
    }
    )glsl";

}

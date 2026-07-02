# Shader Parameters

This document provides a list of all parameters that are passed by the rendering engine to the shader


## Vertex data

This is a list of all vertex shader parameters and their contained data

Vertex position:
```glsl
layout(location = 0) in vec3 aPos;
```

Vertex normal
```glsl
layout(location = 0) in vec3 aNorm;
```

UV coordinates
```glsl
// TODO
```

## Position uniforms

This is a list of all position matrix shader parameters and their contained data

Model matrix (Provided per RenderCommand, Some default objects may not support the model matrix)
```glsl
uniform mat4 uModelMatrix;
```

View matrix / Camera matrix
```glsl
uniform mat4 uViewMatrix;
```

Projection matrix
```glsl
uniform mat4 uProjectionMatrix;
```


## Material data

**Basic Properties**

```glsl
struct MaterialProperties{
    int materialType;
    float diffuse;
    float specular;
    float shininess;
};

uniform MaterialProperties uMaterialProperties;
```

variables:
* materialType: 0, Colored; 1, Textured;
* diffuse: the diffuse factor
* specular: the specular factor
* shininess: the shininess factor

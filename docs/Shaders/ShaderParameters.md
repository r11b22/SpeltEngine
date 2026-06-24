# Shader Parameters

This document provides a list of all parameters that are passed by the rendering engine to the shader


## Vertex data

TODO

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

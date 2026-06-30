# Compute Shaders

This document outlines how to use and dispatch compute shaders

## Usage

For this example we use the following compute shader settings.

```glsl
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
```

To use a compute shader we first need to create one. We can create one from a file as follows.

When creating the compute shader we also specify the local_work groups for each direction. In this case: x:64, y:1, z:1

```c++
ComputeShader compute = ComputeShader::createFromPath("compute.glsl", 16, 16, 1);
```

Now we will start using the compute shader. Assume the following input

```glsl
layout(rgb8, binding = 0) writeonly uniform image2D output;
```

We will use a texture to serve as the output datatype. Make sure to prime the compute shader before setting any form of state

```c++
// Creation (should be done in advance)
Texture tex{"front", size, size, GL_RGB, GL_RGB8, GL_FLOAT};

// usage
compute.use();
tex.imageBind(0, GL_WRITE_ONLY);
```

If we want to supply the shader with extra uniforms we can use setUniform or any of the type specific ones if needed (setUnformFloat, etc.).

```c++
compute.setUniform({"uOption", 2.0f});
```

Now we can start the compute and get our result

```c++
// Make sure the size represents the amount of pixels/elements in each direction
// For a 2D square image: x: size, y: size, z: 1
// Aditionally rawDispatch requires .use() to be called on the compute
compute.rawDispatch(size, size, 1);

// If we don't need to call use on the compute before dispatching
// This will call use for you
// compute.dispatch(size, size, 1)
```

Dispatching the shader does not mean it is actually finished with its calculations. If we want to have the data on the cpu from the texture we need to wait for the compute shader to finish writing to it.

```c++
compute.waitForFinish();
```


Now the data will be contained inside the Texture and you can get it via the functions on Texture.

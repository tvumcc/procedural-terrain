# Procedural Terrain Generator

An OpenGL 3.3 and C++ program that generates unique landscapes using [Perlin Noise](https://en.wikipedia.org/wiki/Perlin_noise).

![preview](assets/preview.png)

## Building

To build, you must have [CMake](https://cmake.org/) installed.
Start by creating a folder named build:

```bash
mkdir build
cd build
```

Select your generator and build, for Windows I usually use `"Visual Studio 17 2022"` or `"MinGW Makefiles"`, as for Linux, I use `"Unix Makfiles"`.

```bash
cmake -G "Unix Makefiles" .
cmake --build .
```

Make sure that you run the executable from the root of the project or else the shaders and assets will fail to load.

## Libraries

[glfw](https://github.com/glfw/glfw) - for window management and input handling
[glad](https://glad.dav1d.de/) - for loading OpenGL functions
[glm](https://github.com/g-truc/glm) - for linear algebra
[stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) - for image loading
[imgui](https://github.com/ocornut/imgui) - for GUI
[FastNoiseLite](https://github.com/Auburn/FastNoiseLite) - for the GLSL implementation of Perlin Noise
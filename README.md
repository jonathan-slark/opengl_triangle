# OpenGL Triangle Example (C99)

A minimal OpenGL example that renders a triangle using modern OpenGL 4.6 and SPIR-V shaders, written in C99. This is a C implementation of the [Learn OpenGL](https://learnopengl.com/Getting-started/Hello-Triangle) tutorial, which was originally written in C++.

![Triangle Screenshot](screenshot.png)

## Features

- Modern OpenGL 4.6
- Written in C99 (not C++)
- SPIR-V shader compilation
- GLFW for window management
- Debug output support
- Minimal dependencies

## Build Requirements

- MSYS2 environment
- OpenGL 4.6 capable GPU
- glslc.exe from Vulkan SDK (for shader compilation)

## Building

1. Install MSYS2 and required packages:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-glfw
```

2. Clone the repository and build:
```bash
make
```

3. Run the example:
```bash
make run
```

## Project Structure
```
├── triangle.c        # Main source file
├── glad.h            # OpenGL loader
├── config.h          # Configuration constants
├── Makefile          # Build system
└── shaders/          # GLSL shader files
    ├── vertex.glsl   # Vertex shader
    └── fragment.glsl # Fragment shader
```

## Shader Compilation

The project uses SPIR-V shaders, which are compiled from GLSL using glslc. The Makefile handles this automatically. The shaders are:

- vertex.glsl: Defines vertex positions
- fragment.glsl: Sets the triangle color

## How It Works

1. Initializes GLFW and creates an OpenGL 4.6 context
2. Loads SPIR-V shaders and creates shader program
3. Sets up vertex buffer and vertex array object
4. Renders a triangle in the render loop
5. Handles window resizing and keyboard input

### Key bindings:

- ESC: Close window

## Debug Support

When built without -DNDEBUG, the program enables OpenGL debug output for easier development and troubleshooting.

## Postmortem

I initially attempted to create an OpenGL context using Win32, but the process quickly became frustrating. The challenges—such as having to create a dummy context before the real one due to limited documentation—led me to switch to GLFW. With GLFW handling the low-level OS intricacies, the setup became much simpler and naturally cross-platform.

I chose to reuse SPIR-V shaders, following the approach from the vulkan-triangle project. Keep in mind that this requires an OpenGL 4.6 capable GPU, which is an important consideration.

Compared to vulkan-triangle, opengl-triangle not only has significantly fewer lines of code but also abstracts much of the hardware complexity. This makes it a more accessible option if you don't need the full power and granularity of Vulkan.

## Credits

- Based on [Learn OpenGL](https://learnopengl.com/Getting-started/Hello-Triangle) by Joey de Vries
- Uses [GLAD](https://github.com/Dav1dde/glad) for OpenGL loading
- Uses [GLFW](https://www.glfw.org/) for window management

## License

This project is licensed under the MIT License - see the LICENSE.txt(LICENSE.txt) file for details.

# CGP - Computer Graphics Programming Library

<a name="Introduction"></a>

## Introduction

[CGP - Computer Graphics Programming -](https://imagecomputing.net/cgp/index.html) is a lightweight and minimalist C++ library using OpenGL to represent, animate, and interact with 3D scenes. 

It features a set of simple structures and functions (vectors, matrices, mesh structures, transforms, camera, etc) that are simple to use and read. The objective is to save time compared to raw OpenGL coding, while preserving the fundamental logic and comprehension of high-performance Graphics. The 3D scene can be exported as a Webpage via Emscripten scripts.

The main objective of CGP is to provide
* **Easy to use CG basic tools**. 
* **Data and structure ready for simple interaction and animation**. 
* **Helping structures and functions that do not impose a framework**.
* **Easy to read and understand source code**. 
* **Secured and easy to debug code**. 

### Additional documentation

* **[Library Webpage](https://imagecomputing.net/cgp/index.html)**

* **[Code Examples](https://imagecomputing.net/cgp/documentation/07_examples/index.html)**, [github-page](https://github.com/drohmer/cgp_examples)


<a name="Content"></a>

## Content of the repository

This repository is a ready-to-compile project: the library and a minimal 3D scene that uses it. Copy the folder to start a new project; there is nothing to install system-wide.

```text
cgp/                  The CGP library, with its third-party code (GLAD, GLFW, ImGui, ...)
src/                  The program: main.cpp, application, environment, scene (edit scene.cpp/.hpp)
shaders/              Default shaders (mesh/ and single_color/), loaded at startup
scripts/              visual-studio-generate.bat and emscripten/ (web build)
test/                 Unit tests of the GL-free modules
CMakeLists.txt        CMake project (all platforms, and Emscripten)
CMakePresets.json     CMake presets: debug, release, relwithdebinfo
Makefile              Alternative to CMake on Linux and macOS
```

Every `.cpp` file under `src/` and `cgp/` is compiled.


<a name="Compilation"></a>

## Compile and run

CGP requires
* A C++17 compatible compiler (GCC/Clang, or a recent Visual Studio).
* An OpenGL 3.3 (or greater) compatible system.
* [CMake](https://cmake.org/) 3.16 or greater (or only `make` on Linux and macOS). The CMake presets use [Ninja](https://ninja-build.org/).
* [libGLFW](https://www.glfw.org/) and [pkgconfig](https://www.freedesktop.org/wiki/Software/pkg-config/) installed on Linux and macOS (a precompiled GLFW is provided for Windows).

```sh
# Linux (Debian / Ubuntu)
sudo apt-get install build-essential pkg-config cmake libglfw3-dev ninja-build
# macOS (Homebrew)
brew install cmake pkg-config ninja glfw
```

**Linux and macOS**, with the CMake presets (`debug`, `release`, `relwithdebinfo`):

```sh
cmake --preset release
cmake --build --preset release
./project
```

or with the Makefile only:

```sh
make -j8
./project
```

**Windows**: run `scripts/visual-studio-generate.bat`, open the generated solution in `build/`, then compile and run with "Local Windows Debugger".

Run the program from the root of the project (or from a build folder inside it): it looks for the `shaders/` folder at startup. On macOS, the CMake option `-DMACOS_GLFW_PRECOMPILED=ON` uses the precompiled GLFW of `cgp/third_party/glfw/macos/` instead of the Homebrew one.

**Web (Emscripten)**: with the [Emscripten SDK](https://emscripten.org/) activated,

```sh
mkdir build_em
cd build_em
emcmake cmake ..
emmake make
emrun index.html
```

**Unit tests** (modules that do not need OpenGL):

```sh
cmake -S test -B build_test
cmake --build build_test
./build_test/cgp_test
```

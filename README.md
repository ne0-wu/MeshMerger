## How to build

This project uses `vcpkg` to manage its dependencies. Please ensure that the `vcpkg` executable is added to your system `PATH` and set the `VCPKG_ROOT` environment variable to the path of your `vcpkg` directory.

To install the dependencies, run the following command:

```shell
$ vcpkg install glad glm glfw3 imgui[glfw-binding,opengl3-binding] eigen3 openmesh
```

We recommend using VS Code as the IDE for this project. To build the project, add the following configuration to the `settings.json` file:

```json
{
    "cmake.configureSettings": {
        "CMAKE_TOOLCHAIN_FILE": "${env:VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    }
}
```

Then, follow these steps:
1. Press `Ctrl+Shift+P` and select `CMake: Configure` to configure the project.
2. Press `Ctrl+Shift+P` and select `CMake: Build` to build the project.

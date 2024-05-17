## Installation

To install this program, just do the following commands:

    mkdir build
    cd build
    cmake ..
    cmake --build . --install target

The executable will be generated under src.

# SDL 3D-VisuPIP

This project is based on a simplified 3D graphical pipeline introduced in the Computer Graphics course taken in Universidade Estadual do Oeste do Paraná (Unioeste).
Overall, the pipeline consists of _four_ main steps: **Camera**, **Parallel/Perspective Projection**, **Cutout (Z-Buffer)** and **Shading**.

## Stack

This project was built and configured by **CMake**, a powerful building, compiling and packaging tool made for C/C++. More information on the official website https://cmake.org/.

The rendering and graphical parts of the pipeline were made utilizing **SDL2**, a graphical framework that works as an API for OpenGL and recently, Vulkan. SDL2 facilitates many functions which are common in between operational systems. https://github.com/libsdl-org/SDL

However, it is also utilized **Imgui** for the graphical U.I. Imgui is presented as a "bloat-free" user interface framework that works seamlessly with many different graphical frameworks, including SDL2. This library was essential for building quick and responsive user interface. https://github.com/ocornut/imgui

Lastly, it was also made use of the other open source project denominated Eigen. It works as a linear algebra library, being extremly useful for vector and matrix calculations. https://eigen.tuxfamily.org/index.php?title=Main_Page

## CMake

This is one of the few build tools for C/C++ available that covers most necessities of C++ developers, I must also say, it is the most complicated one as well.
This project utilizes very simple directives given to compile it. CMake is cross-platform and hence, much of the work is done automatically by itself.  
Mind that as my first CMake project, my knowledge of CMake is few and my description should be considered humbly.

CMake-configured projects normally are defined by various **CMakeLists.txt** files, that gives different instructions to how the project should be built and installed, plus many other features.

### Configuring a Project

Every CMake project has a main *CMakeLists.txt* that has (should have) the following lines:

    cmake_minimum_required(VERSION X.X.X)
    project(project_name VERSION X.X.X)

The first command specifies the minimum version of CMake installed in the operational system that is required in order to configure the current project. The _project_ is also straight foward and defines a name for the project, and the its current version. Most commands generate variables that can be utilized later, for an instance, we can always refer back to the project's defined name by utilizing **${CMAKE_PROJECT_NAME}**.

More detailed documentation can be found in https://cmake.org/cmake/help/latest/.

### Adding an Executable

Most simple projects will consist of a command of the following:

    add_executable(
    MyExecutable
    main.hpp
    main.cpp)

This command specifies an executable to created utilizing different header and source files. *MyExecutable* and be reffered just like that in the following commands to specify this executable once again.

### Linking & Libraries

Finally, libraries an important artifcat in most projects. Be it one made by yourself, or an external dependency your project required. CMake provides many different ways in which libraries and be automatically configured.

    add_library(library_name TYPE
        sources)

The add library lets you specify a library name that will be utilized later on. Source files are specified such as .hpp and .cpp files, the _TYPE_ argument specifies wheter the library is STATIC, SHARED or a MODULE. STATIC means compiled object files of the library are linked directly into the target. SHARED mean it's dynamic and can be loaded at runtime.

    target_include_directories(target_name INTERFACE|PUBLIC|PRIVATE include_path)

CMake does not know where to search for when compiling a target, for this reason this command specifies the folders in which the target are located. Target could be a library you specified or an executable.

    target_link_libraries(target INTERFACE|PUBLIC|PRIVATE libraries)

This command lets you specify which libraries a target requires.

    add_subdirectory(relative_path)

Just adds a subdirectory that will be used in the project, that subdirectory must have another CMakeLists.txt. Therefore you can make a chain CMakeLists.txt flux, helps organizing the project.
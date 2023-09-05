# Stimply-C

I am currently developing this on Linux, and using CMake as a build system.

You NEED to have GLFW installed on your machine (Linux) and VULKAN_SDK needs to be setup in your path(Linux/Windows).

I am not running this on windows but i think it should be straight forward once you create your Visual Studio project and link to glfw.

On linux, simply go to the project's dir and type cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug and then cmake --build ./build

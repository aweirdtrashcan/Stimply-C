#!/bin/bash
# Build script for engine
set echo on

mkdir -p ../bin

currentDir=$(pwd)

cd ./src/

# Get a list of all the .c files.
cFilenames=$(find . -type f -name "*.c")

# echo "Files:" $cFilenames

assembly="StimplyEngine"
compilerFlags="-g -fdeclspec -fPIC"
# -fms-extensions 
# -Wall -Werror
includeFlags="-Isrc -I$VULKAN_SDK/include"
linkerFlags="-lvulkan -lglfw -L$VULKAN_SDK/lib"
defines="-D_DEBUG -DS_LINUX_"

echo "Building $assembly..."
clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags

cd $currentDir
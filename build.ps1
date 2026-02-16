#Path to vcpkg 
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" -A x64
cmake --build . --config Debug
cmake_minimum_required(VERSION 3.28)
project(C_project C)

set(CMAKE_C_COMPILER "/opt/homebrew/Cellar/gcc/14.1.0_1")

add_executable(C_project main.c c_extension.c)

include_directories(
        /Library/Frameworks/Python.framework/Versions/3.12/include/python3.12
        /opt/homebrew/opt/llvm/include
)

link_directories(
        /opt/homebrew/opt/llvm/lib
        /Library/Frameworks/Python.framework/Versions/3.12/lib
)

# Find the OpenMP package
find_package(OpenMP)
if (OpenMP_C_FOUND)
    target_link_libraries(C_project PUBLIC OpenMP::OpenMP_C)
endif()

# Link Python library specifically to your executable
target_link_libraries(C_project PRIVATE "/Library/Frameworks/Python.framework/Versions/3.12/lib/libpython3.12.dylib")  # Adjust the path and filename as foundbrew
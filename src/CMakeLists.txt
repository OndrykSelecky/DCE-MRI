# This is the root ITK CMakeLists file.
cmake_minimum_required(VERSION 2.8.9)

project(DCE-MRI)

SET(ITK_DIR "D:/Program Files/ITK/Build")
# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})


add_executable(DCE-MRI DCE-MRI.cxx )

target_link_libraries(DCE-MRI ${ITK_LIBRARIES})

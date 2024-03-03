# CMakeList.txt : CMake project for DependencyInjection, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_library (DependencyInjection "DependencyInjection.cpp" "Container.cpp" "DependencyInjection.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DependencyInjection PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

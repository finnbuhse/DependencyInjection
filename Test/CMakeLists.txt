# CMakeList.txt : CMake project for DependencyInjection, include source and define
# project specific logic here.
#
# Setup testing

enable_testing()
include_directories("../DependencyInjection" "dependencies/googletest/googletest/include")

# Add source to this project's executable.
add_executable (RunDependencyInjectionTests "TypeRegistryTest.cpp" "DependentTest.cpp" "ContainerTest.cpp")

# Link test executable against gtest & gtest_main
target_link_directories(RunDependencyInjectionTests PRIVATE D:/Source/C++/Current/DependencyInjection/out/DependencyInjection/Debug D:/Source/C++/Current/DependencyInjection/Test/dependencies/googletest/out/lib/Debug)
target_link_libraries(RunDependencyInjectionTests DependencyInjection gtest gtest_main)

add_test(
    NAME runUnitTests
    COMMAND RunDependencyInjectionTests
)


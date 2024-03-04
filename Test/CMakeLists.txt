# CMakeList.txt : CMake project for DependencyInjection, include source and define
# project specific logic here.
#
# Setup testing

enable_testing()
include_directories("../DependencyInjection" "dependencies/googletest/googletest/include")

# Add source to this project's executable.
add_executable (RunDependencyInjectionTests "TypeRegistryTest.cpp" "DependentTest.cpp" "ContainerTest.cpp")

set(GTEST_LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/googletest/out/lib/Debug)

# Link test executable against gtest & gtest_main
target_link_directories(RunDependencyInjectionTests PRIVATE ${DI_LIBRARY_PATH} ${GTEST_LIBRARY_PATH})
target_link_libraries(RunDependencyInjectionTests DependencyInjection gtest gtest_main)

add_test(
    NAME runUnitTests
    COMMAND RunDependencyInjectionTests
)


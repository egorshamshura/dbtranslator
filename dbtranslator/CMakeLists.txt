cmake_minimum_required(VERSION 3.31)

project(dbtranslator)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(third-party)

file(GLOB_RECURSE DBTRANSLATOR_SOURCE CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} ${DBTRANSLATOR_SOURCE})

find_package(elfio CONFIG REQUIRED)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

llvm_map_components_to_libnames(
  LLVM_LIBRARIES
  Core
  OrcJIT
  Support
  nativecodegen
  support
  core
  irreader
  orcjit
  native
  x86asmparser
)


target_link_libraries(${PROJECT_NAME} PUBLIC elfio ${LLVM_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC include/dbtranslator ${LLVM_INCLUDE_DIRS})

file(GLOB DBTRANSLATOR_TESTS_SOURCE CONFIGURE_DEPENDS tests/*.cpp)
add_executable("${PROJECT_NAME}-tests" ${DBTRANSLATOR_TESTS_SOURCE})
target_link_libraries("${PROJECT_NAME}-tests" PUBLIC ${PROJECT_NAME})

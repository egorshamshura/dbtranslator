include(FetchContent)

message(STATUS "Fetching argparse...")
FetchContent_Declare(
  argparse
  URL https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.tar.gz
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(argparse)

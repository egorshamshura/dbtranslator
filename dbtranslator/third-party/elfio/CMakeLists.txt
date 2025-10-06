include(FetchContent)

message(STATUS "Fetching elfio...")
FetchContent_Declare(
  elfio
  URL https://github.com/serge1/ELFIO/releases/download/Release_3.12/elfio-3.12.tar.gz
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(elfio)

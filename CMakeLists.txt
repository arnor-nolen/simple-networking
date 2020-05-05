cmake_minimum_required (VERSION 3.16)
project(simple-networking)

# Set binary output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)

# Downloading cmake-conan script 
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(
    STATUS
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
       "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_add_remote(NAME bincrafters URL
                 https://api.bintray.com/conan/bincrafters/public-conan)

# Installing dependencies
conan_cmake_run(
  REQUIRES
  boost/1.73.0
  BASIC_SETUP
  CMAKE_TARGETS
  BUILD
  missing)

# Set platform-specific stuff
if (MSVC)
    add_compile_options(/W4)
    # We're targeting Windows 10 only, to suppress Boost.Asio warning
    add_definitions(-D _WIN32_WINNT=0x0A00)
    # set(EXTRA_LINKER_FLAGS "")
elseif(APPLE)
    add_compile_options(-g -Wall -Wextra -Wpedantic -Wshadow)
    # set(EXTRA_LINKER_FLAGS boost_program_options-mt)
else()
    message(FATAL_ERROR "Currently, only MSVC and APPLE architectures are officially supported. You need to link your libraries manually by editing CMakeLists.txt")
endif()

add_executable (server src/server.cpp)
add_executable (client src/client.cpp)
target_precompile_headers(server PUBLIC src/pch.h)
target_precompile_headers(client PUBLIC src/pch.h)
# Use c++17 standard
target_compile_features(server PUBLIC cxx_std_17)
target_compile_features(client PUBLIC cxx_std_17)
target_link_libraries(server CONAN_PKG::boost)
target_link_libraries(client CONAN_PKG::boost)
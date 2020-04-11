cmake_minimum_required (VERSION 3.10)
project(simple-networking)

# Include and library paths
include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

# Find dependencies 

# Set binary output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<0:>)


# Set platform-specific stuff
if (WIN32)
    # Add "-mwindows" flag for hiding the console
    set(EXTRA_LINKER_FLAGS ws2_32 mswsock)
elseif(APPLE)
    # For debugging
    add_compile_options(-g)
    set(EXTRA_LINKER_FLAGS "")
else()
    message(FATAL_ERROR "Currently, only WIN32 and APPLE architectures are officially supported. You need to link your libraries manually by editing CMakeLists.txt")
endif()

add_executable (server src/server.cpp)
add_executable (client src/client.cpp)
# Use c++17 standard
target_compile_features(server PUBLIC cxx_std_17)
target_link_libraries(server ${EXTRA_LINKER_FLAGS})
target_compile_features(client PUBLIC cxx_std_17)
target_link_libraries(client ${EXTRA_LINKER_FLAGS})
# Find Zephyr. This also loads Zephyr's build system.
cmake_minimum_required(VERSION 3.20.0)

# Set Zephyr environment
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)

# Add application src directory to include path for modules (e.g., datastoreMeta.h)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add your source file to the "app" target. This must come after
# find_package(Zephyr) which defines the target.
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/listSourcesAndIncludes.cmake)

set(SRC "")
set(INC "")

listSources(${CMAKE_CURRENT_SOURCE_DIR}/src SRC)
listIncludesDir(${CMAKE_CURRENT_SOURCE_DIR}/src INC)

# message("SRC: ${SRC}")
# message("INC: ${INC}")

target_sources(app PRIVATE ${SRC})
target_include_directories(app PRIVATE ${INC})

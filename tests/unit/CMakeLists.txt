# Find Zephyr. This also loads Zephyr's build system.
cmake_minimum_required(VERSION 3.20.0)

set(BOARD "qemu_cortex_m0")

set(CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../prj.conf)

# Set Zephyr environment
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)

# Add your source file to the "app" target. This must come after
# find_package(Zephyr) which defines the target.
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/listUnitTestSourcesAndIncludes.cmake)

set(SRC "")
set(INC "")

getFileListForSuite(SRC INC)

# message("SRC: ${SRC}")
# message("INC: ${INC}")

target_sources(app PRIVATE ${SRC})
target_include_directories(app PRIVATE ${INC})

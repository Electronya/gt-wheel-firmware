# Find Zephyr. This also loads Zephyr's build system.
cmake_minimum_required(VERSION 3.20.0)

# If no build environment is define, use the prod one
if(NOT DEFINED BUILD_ENV)
  set(BUILD_ENV "prod")
endif()

# Setting target board
if((BUILD_ENV STREQUAL "prod") OR (BUILD_ENV STREQUAL "dev"))
  set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
  set(BOARD "enya_gt_wheel")
elseif(BUILD_ENV STREQUAL "qemu")
  set(BOARD "qemu_cortex_m0")
else()
  message(FATAL_ERROR "Unsupported build environment!!!")
endif()

# Load the application config for development
if((BUILD_ENV STREQUAL "dev") OR (BUILD_ENV STREQUAL "qemu"))
  set(CONF_FILE "prj_dev.conf")
endif()


# Set Zephyr environment
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)

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

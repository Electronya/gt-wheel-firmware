#!/bin/bash
# This script build the firware through electronya-firm-builder docker container

BUILD_MODE=""
PROD_MODE="prod"
DEV_MODE="dev"
DEBUG_MODE="debug"
QEMU_MODE="qemu"
TEST_MODE="test"
CONFIG_MODE="config"

function redPrint {
  if [[ "$1" != "" ]]
  then
    STRING=$1
    RED='\033[0;31m'
    NC='\033[0m' # No Color
    echo -e "${RED}${STRING}${NC}"
  fi
}

function greenPrint {
  if [[ "$1" != "" ]]
  then
    STRING=$1
    GREEN='\033[0;32m'
    NC='\033[0m' # No Color
    echo -e "${GREEN}${STRING}${NC}"
  fi
}

function printUsage {
  echo "Usage: $0 [-m [${PROD_MODE}|${DEV_MODE}|${DEBUG_MODE}|${QEMU_MODE}|${TEST_MODE}|${CONFIG_MODE}]] <-p>"
}

function exitError {
  ERR_MSG=$1
  redPrint "${ERR_MSG}"
  printUsage
  exit 1
}

function validateModeArg {
  if [[ ! $BUILD_MODE =~ ^($PROD_MODE|$DEV_MODE|$DEBUG_MODE|$QEMU_MODE|$TEST_MODE|$CONFIG_MODE)$ ]]
  then
    exitError "ERROR: ${BUILD_MODE} is not a supported build mode."
  fi
}

function validateOptions {
  # Check for the -m option
  if [ -z "$BUILD_MODE" ]
  then
    exitError "ERROR: No build mode have been provided."
  else
    validateModeArg
  fi
}

function pullZephyrBuilderImage {
  greenPrint "Pulling electronya-firm-builder image..."
  docker pull judebake/electronya-firm-builder:3.4.0 || return 1
  greenPrint "electronya-firm-builder immage pull DONE!!"
}

function buildFirmware {
  greenPrint "Building with ${BUILD_MODE} mode..."
  docker run --name firmware-builder --rm -it \
  --privileged -v /dev/bus/usb:/dev/bus/usb \
  -v "${PWD}:/github/workspace" \
  judebake/electronya-firm-builder:3.4.0 ${BUILD_MODE}\
  || return 1
  greenPrint "Building DONE!!"
}

while getopts ":m:p" option
do
  case "${option}" in
    m)
      BUILD_MODE=${OPTARG}
      ;;
    p)
      pullZephyrBuilderImage || exitError "ERROR: Unable to pull electronya-firm-builder image."
      ;;
    :)
      exitError "ERROR: -${OPTARG} requires an argument."
      ;;
    \?)
      exitError "ERROR: -${OPTARG} is not a supported option."
      ;;
    esac
done

validateOptions
buildFirmware || exitError "ERROR: Unable to build the firmware."

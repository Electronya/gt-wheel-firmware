#!/bin/bash
# This script build the firmware when in the standard Electronya Zephyr Workspace.

WEST_ROOT=$(west topdir)
APP_ROOT=$(pwd)
APP_REL_DIR=$(echo $APP_ROOT | sed "s|$WEST_ROOT/||g")

PRISTINE_MODE="auto"

west config manifest.path $APP_REL_DIR

while getopts ":up:" option
do
  case "${option}" in
    u)
      ./scripts/ws-update.sh
      ;;
    p)
      PRISTINE_MODE=${OPTARG}
      ;;
    \?)
      exitError "ERROR: -${OPTARG} is not a supported option."
      ;;
    esac
done

west build -p $PRISTINE_MODE -b enya_gt_wheel .

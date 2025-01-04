#!/bin/bash
# This script build the firmware when in the standard Electronya Zephyr Workspace.

WEST_ROOT=$(west topdir)
APP_ROOT=$(pwd)
APP_REL_DIR=$(echo $APP_ROOT | sed "s|$WEST_ROOT/||g")

west config manifest.path $APP_REL_DIR

while getopts ":up:" option
do
  case "${option}" in
    u)
      west update
      ;;
    \?)
      exitError "ERROR: -${OPTARG} is not a supported option."
      ;;
    esac
done

rm -rf ./twister-out*
../../zephyr/scripts/twister -T ./tests/unit/

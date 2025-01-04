#!/bin/bash
# This script build the firmware when in the standard Electronya Zephyr Workspace.

WEST_ROOT=$(west topdir)
APP_ROOT=$(pwd)
APP_REL_DIR=$(echo $APP_ROOT | sed "s|$WEST_ROOT/||g")

west config manifest.path $APP_REL_DIR

west update

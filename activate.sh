#!/bin/bash

TOP_DIR=$(west topdir)
CURRENT_DIR=$(pwd)
APP_DIR=$(realpath --relative-to=$TOP_DIR $CURRENT_DIR)

BOARD=b_g474e_dpow1
DEV_BUILD=0

# echo $TOP_DIR
# echo $CURRENT_DIR
# echo $APP_DIR

while getopts "b:d" opt
do
    case ${opt} in
        b)
            BOARD=$OPTARG
            ;;
        d)
            DEV_BUILD=1
            ;;
        ?)
            echo "Unsupported argument"
            exit 1
            ;;
    esac
done

west config --local manifest.path $APP_DIR
west config --local build.board $BOARD

if [ "$DEV_BUILD" -eq 1 ]; then
    west config --local build.cmake-args -- -DEXTRA_CONF_FILE=prj_dev.conf
else
    west config -d --local build.cmake-args || true
fi

west update || exit 1

#!/bin/sh
cd "$(dirname "$0")"

export ADTF_DIR=/opt/adtf/2.14.0
export ADTF_PROJECT_PATH=./LiveVisualizationConfig/LiveVisualizationConfig.prj

if [ -d $ADTF_DIR ]; then
    echo "ADTF dir found."
else
    echo "ADTF dir not found in $ADTF_DIR. Check the path to ADTF."
    exit 1
fi


$ADTF_DIR/bin/adtf_devenv -project=$ADTF_PROJECT_PATH -run

$SHELL




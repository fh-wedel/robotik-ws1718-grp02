#!/bin/sh
cd "$(dirname "$0")"

export ADTF_DIR=/opt/adtf/2.14.0
export ADTF_PROJECT_PATH=./PythonExtIfConfig/PythonExtIfConfig.prj

if [ -d $ADTF_DIR ]; then
    echo "ADTF dir found."
else
    echo "ADTF dir not found in $ADTF_DIR. Check the path to ADTF."
    exit 1
fi
# run Python Thrift Server in new terminal
x-terminal-emulator -e ./../extern/Python_ExtIf_Server/start.sh

# start the ADTF Thrift Client
$ADTF_DIR/bin/adtf_devenv -project=$ADTF_PROJECT_PATH

#the qt filters and the video displays have to be deactivated before
#$ADTF_DIR/bin/adtf_runtime -project=$ADTF_PROJECT_PATH -minimized -run



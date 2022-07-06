#!/bin/sh

if [ $# -lt 2 ]
then
	echo "Number of script parameters less than 2"
    exit 1
else
	WRITEFILE=$1
    touch ${WRITEFILE}
    if [ $? -eq 0 ]
    then
    	WRITESTR=$2
    	echo "$WRITESTR" > ${WRITEFILE}
    	exit 0
    else
    	exit 1
    fi
fi
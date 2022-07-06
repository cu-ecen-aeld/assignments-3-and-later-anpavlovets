#!/bin/sh

NUMFILES=0
NUMLINES=0

if [ $# -lt 2 ]
then
	echo "Number of script parameters less than 2"
    exit 1
else
	FILESDIR=$1
    if [ -d "$FILESDIR" ]
    then
		SEARCHSTR=$2
        for num in `grep -r -h -c ${SEARCHSTR} ${FILESDIR}`
        do
          	a=${num}
        	NUMLINES=$((NUMLINES+a))
            if [ $a -gt 0 ]
            then
               NUMFILES=$((NUMFILES+1))
            fi
        done
        echo "The number of files are $NUMFILES and the number of matching lines are $NUMLINES"
        exit 0
    else
    	echo "${FILESDIR} is not a directory"
    	exit 1
    fi
fi
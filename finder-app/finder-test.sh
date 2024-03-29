#!/bin/sh
# Tester script for assignment 1 and assignment 2
# Author: Siddhant Jajoo

set -e
set -u

NUMFILES=10
WRITESTR=AELD_IS_FUN
WRITEDIR=/tmp/aeld-data
#corrections for assignment-4 Buildroot
#username=$(cat conf/username.txt)
if [ -d /etc/finder-app ]
then
	username=$(cat /etc/finder-app/conf/username.txt)
else
	username=$(cat conf/username.txt)
fi

if [ $# -lt 2 ]
then
	echo "Using default value ${WRITESTR} for string to write"
	if [ $# -lt 1 ]
	then
		echo "Using default value ${NUMFILES} for number of files to write"
	else
		NUMFILES=$1
	fi	
else
	NUMFILES=$1
	WRITESTR=$2
fi

MATCHSTR="The number of files are ${NUMFILES} and the number of matching lines are ${NUMFILES}"

echo "Writing ${NUMFILES} files containing string ${WRITESTR} to ${WRITEDIR}"

rm -rf "${WRITEDIR}"
mkdir -p "$WRITEDIR"

#The WRITEDIR is in quotes because if the directory path consists of spaces, then variable substitution will consider it as multiple argument.
#The quotes signify that the entire string in WRITEDIR is a single string.
#This issue can also be resolved by using double square brackets i.e [[ ]] instead of using quotes.
if [ -d "$WRITEDIR" ]
then
	echo "$WRITEDIR created"
else
	exit 1
fi

#echo "Removing the old writer utility and compiling as a native application"
#make clean
#make

#corrections for assignment-4 Buildroot

#for i in $( seq 1 $NUMFILES)
#do
##	./writer.sh "$WRITEDIR/${username}$i.txt" "$WRITESTR"
#        ./writer "$WRITEDIR/${username}$i.txt" "$WRITESTR"
#done

#OUTPUTSTRING=$(./finder.sh "$WRITEDIR" "$WRITESTR")

which writer > /dev/null 2>&1
if [ $? -eq 0 ]
then
	for i in $( seq 1 $NUMFILES)
	do
		writer "$WRITEDIR/${username}$i.txt" "$WRITESTR"
	done
else
	for i in $( seq 1 $NUMFILES)
        do
                ./writer "$WRITEDIR/${username}$i.txt" "$WRITESTR"
        done
fi

which finder.sh > /dev/null 2>&1
if [ $? -eq 0 ]
then
	OUTPUTSTRING=$(finder.sh "$WRITEDIR" "$WRITESTR")
else
	OUTPUTSTRING=$(./finder.sh "$WRITEDIR" "$WRITESTR")
fi

echo ${OUTPUTSTRING} > /tmp/assignment-4-result.txt
#end of corrections

set +e
echo ${OUTPUTSTRING} | grep "${MATCHSTR}"
if [ $? -eq 0 ]; then
	echo "success"
	exit 0
else
	echo "failed: expected  ${MATCHSTR} in ${OUTPUTSTRING} but instead found"
	exit 1
fi

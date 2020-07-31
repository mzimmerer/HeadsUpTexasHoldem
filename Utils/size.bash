#!/bin/bash

PWD=`dirname $0`

TOTAL=0
OBJ_DIR=$PWD/../Obj
OBJ_FILES=`find $OBJ_DIR | grep "\.o"`
for FILE in $OBJ_FILES
do
    OUTPUT=`avr-size $FILE`
	COUNT=0
	for ENTRY in $OUTPUT
	do
		if [[ $COUNT == 6 ]] ; then
		    FILE_SIZE="${ENTRY}"
			TOTAL=`expr $TOTAL + $ENTRY`
		fi
		if [[ $COUNT == 11 ]] ; then
		    FILE_NAME="`basename $ENTRY`"
			FILE_NAME=`echo "${FILE_NAME}" |tr -d '\r'`
		fi		
		COUNT=`expr $COUNT + 1`
	done
	echo "${FILE_NAME}: ${FILE_SIZE}b"
done
echo "Total: ${TOTAL}b"

avr-size $PWD/../texas_holdem.elf

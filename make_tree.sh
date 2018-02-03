#!/bin/sh

if [ $# -ne 1 -a $# -ne 2 ]
then
    echo \
    "useage:make_tree.sh [RunNo start] [RunNo stop]
    or make_tree.sh [RunNo] (only 1 Run analysis)"
    exit
fi

#####  Dirctory Definition #######
## (change directory as you like) ##
DATA_DIR=./data
ROOT_DIR=./root
MAKETREE_DIR=.
##################################


if [ $# -eq 1 ]
then
    FILE_NAME=`printf "MUSE%06d_*" $1`
    DATA_ALL=`ls ${DATA_DIR}/${FILE_NAME}.edb`
    for RAW_DATA in ${DATA_ALL}
    do
	ROOT_FILE=${RAW_DATA}.root
	if [ -e ${RAW_DATA} ]
	then
	    echo "analyzing Run$1..."
	    ${MAKETREE_DIR}/test_make_tree ${RAW_DATA}
	    mv ${ROOT_FILE} ${ROOT_DIR}/
	    echo "create ${ROOT_FILE##*/}"
	else
	    echo "Run$1 dose not exist..."
	fi
    done
    
else
    RunNo=$1
    while [ ${RunNo} -le $2 ]
    do 
	FILE_NAME=`printf "MUSE%06d_*" ${RunNo}`
	DATA_ALL=`ls ${DATA_DIR}/${FILE_NAME}*/${FILE_NAME}.edb`
	for RAW_DATA in ${DATA_ALL}
	do
	    ROOT_FILE=${RAW_DATA}.root
	    if [ -e ${RAW_DATA} ]
	    then
		echo "analyzing Run${RunNo}..."
		${MAKETREE_DIR}/test_make_tree ${RAW_DATA}
		echo "create ${ROOT_FILE##*/}"
	    else
		echo "Run${RunNo} dose not exist..."
	    fi
done
mv ${ROOT_FILE} ${ROOT_DIR}/
RunNo=`echo "${RunNo}+1"|bc -l` 
done
fi
#!/bin/bash
echo "Decoding raw data files"
echo "Input, output, complete file or not (0 or 1), ROF number"
RAWDATANAME=$1
ROOTNAME=$2
BOOLCOMPLETE=$3
ROFNUMBER=$4
current_time=$(date "+%y%m%d_%H%M%S")

if [[ -n "$RAWDATANAME" ]]; then
    if [ "$BOOLCOMPLETE" = 1 ]; then
	root.exe -l -b -q macro/run_rawdecoding_mft.C\(\"/localhome/mft/alice/data_saclay_new/$RAWDATANAME\",\"/localhome/mft/alice/O2/output_raw/$ROOTNAME\_$current_time.root\",1\) > $ROOTNAME\_$current_time.out
    fi
    if [ "$BOOLCOMPLETE" = 0 ]; then
	 root.exe -l -b -q macro/run_rawdecoding_mft.C\(\"/localhome/mft/alice/data_saclay_new/$RAWDATANAME\",\"/localhome/mft/alice/O2/output_raw/$ROOTNAME\_ROF$ROFNUMBER\_$current_time.root\"\) > $ROOTNAME\_ROF$ROFNUMBER\_$current_time.out
    fi
fi

echo "Done!!!"
echo "Let's see the digits"

root.exe -l -b -q macro/mapping.C+ | tee mapping\_$current_time.out
rm core*

echo "Chips name analysis done!!!"

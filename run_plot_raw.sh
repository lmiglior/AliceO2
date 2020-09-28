#!/bin/bash
echo "Decoding raw data files"
#echo "Input, output, complete file or not (0 or 1), ROF number"
RAWDATANAME=$1
#ROOTNAME=$2
DATARAW=$2
#BOOLCOMPLETE=$3
#ROFNUMBER=$4
current_time=$(date "+%Y_%m_%d__%H_%M_%S")
starttime=$(date +"%H.%M.%S,%3N")
#ROOTNAMEDEF=$ROOTNAME\_$current_time
#ROOTNAMEDEFROF=$ROOTNAME\_ROF$ROFNUMBER\_$current_time
#RAWDATANAME=$RAWDATANAME1.raw

#echo "Decompressed raw data"
#lz4 -d /home/o2flp/alice/data_raw/$RAWDATANAME1 /home/o2flp/alice/data_raw/$RAWDATANAME
echo "Starting at $starttime" 
echo "Decoding raw data"
#if [ "$BOOLCOMPLETE" = 1 ]; then
root.exe -l -b -q macro/run_rawdecoding_mft.C+\(\"/home/o2flp/alice/data_raw/test_cern/$RAWDATANAME\",\"/home/o2flp/alice/output_raw/$RAWDATANAME.root\",1\) > $RAWDATANAME\_$current_time.out
#fi
# if [[ "$BOOLCOMPLETE" = 0 ]]; then
# 	 root.exe -l -b -q macro/run_rawdecoding_mft.C+\(\"/home/o2flp/alice/data_raw/test_cern/$RAWDATANAME\",\"/home/o2flp/alice/output_raw/$ROOTNAMEDEFROF.root\",0,$ROFNUMBER\) > $ROOTNAME\_ROF$ROFNUMBER\_$current_time.out
# fi

rawtime=$(date +"%H.%M.%S,%3N")
echo "Done at $rawtime!!!"
root.exe -l -b -q macro/mapping_v2.c+ | tee mapping\_$current_time.out
echo "Let's analyse digits : $current_time"
#if [ "$BOOLCOMPLETE" = 1 ]; then
root -l -b -q macro/PlotRawDec.C\(\"/home/o2flp/alice/output_raw/$RAWDATANAME.root\",\"$DATARAW\",\"$current_time\"\)
#fi
# if [ "$BOOLCOMPLETE" = 0 ]; then
#     root -l -b -q  macro/PlotRawDec.C\(\"/home/o2flp/alice/output_raw/$ROOTNAMEDEFROF.root\",\"$current_time\"\)
# fi

#rm *.out

finishtime=$(date +"%H.%M.%S,%3N")

echo "Done at time $finishtime"

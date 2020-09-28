#!/bin/bash
echo "Decoding raw data files"
#echo "Input, output, complete file or not (0 or 1), ROF number, VPULSEL"
RAWDATANAME=$1
#DATARAW=$2
#ROOTNAME=$2
#BOOLCOMPLETE=$3
#ROFNUMBER=$4
VPULSEL=$2
current_time=$(date "+%y%m%d_%H%M%S")
starttime=$(date +"%H.%M.%S,%3N")
#ROOTNAMEDEF=$ROOTNAME\_$current_time
#ROOTNAMEDEFROF=$ROOTNAME\_ROF$ROFNUMBER\_$current_time
#RAWDATANAME=$RAWDATANAME1.raw
#echo "Decompressed raw data"
#lz4 -d /home/o2flp/alice/data_raw/$RAWDATANAME1 /home/o2flp/alice/data_raw/$RAWDATANAME
echo "Starting at $starttime" 
echo "Decoding raw data"
#if [ "$BOOLCOMPLETE" = 1 ]; then
 #   echo "1 "
root.exe -l -b -q macro/run_rawdecoding_mft.C+\(\"/home/o2flp/alice/data_raw/test_cern/$RAWDATANAME\",\"/home/o2flp/alice/output_raw/$RAWDATANAME.root\",1\) > $RAWDATANAME\_$current_time.out
#fi
# if [[ "$BOOLCOMPLETE" = 0 ]]; then
#   #  echo " 0 "
# 	 root.exe -l -b -q macro/run_rawdecoding_mft.C+\(\"/home/o2flp/alice/data_raw/test_lyon/h1/thr/$RAWDATANAME\",\"/home/o2flp/alice/output_raw/$ROOTNAMEDEFROF.root\",0,$ROFNUMBER\) > $ROOTNAME\_ROF$ROFNUMBER\_$current_time.out
# fi
rawtime=$(date +"%H.%M.%S,%3N")
echo "Done at $rawtime!!!"
echo "Let's analyse digits : $current_time"
root.exe -l -b -q macro/mapping_v2.c+ | tee mapping\_$current_time.out
echo "Used charge : $VPULSEL"
echo "Chips name analysis done!!!"
digittime=$(date +"%H.%M.%S,%3N")
echo "Digit analysis started at $digittime"
#if [ "$BOOLCOMPLETE" = 1 ]; then
root -l -b -q macro/RawDec.C\(\"/home/o2flp/alice/output_raw/$RAWDATANAME.root\",$VPULSEL\)
#fi
# if [ "$BOOLCOMPLETE" = 0 ]; then
#     root -l -q macro/RawDec.C\(\"/home/o2flp/alice/output_raw/$ROOTNAMEDEFROF.root\",$VPULSEL\)
# fi
#CHARGEINJ=`expr 170 - $VPULSEL`
if [  `expr $VPULSEL` -lt 10 ]; then
    DIGITFILE=digit_coordinates_0$VPULSEL.txt
    echo "Creation de txt file $DIGITFILE"
    mv digit_coordinate.txt macro/$DIGITFILE
    mv macro/$DIGITFILE Thr/test_cern/$DIGITFILE
else 
    DIGITFILE=digit_coordinates_$VPULSEL.txt
    echo "Creation de txt file $DIGITFILE"
    mv digit_coordinate.txt macro/$DIGITFILE
    mv macro/$DIGITFILE Thr/test_cern/$DIGITFILE
fi
#rm /home/o2flp/alice/output_raw/*.root
#rm *.out
finishtime=$(date +"%H.%M.%S,%3N")

echo "Done at time $finishtime"


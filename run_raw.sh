#!/bin/bash
echo "Decoding raw data files"
echo "Input, output, complete file or not (0 or 1), ROF number, VPULSEL"
RAWDATANAME=$1
ROOTNAME=$2
BOOLCOMPLETE=$3
ROFNUMBER=$4
VPULSEL=$5

current_time=$(date "+%y%m%d_%H%M%S")

ROOTNAMEDEF=$ROOTNAME\_$current_time
ROOTNAMEDEFROF=$ROOTNAME\_ROF$ROFNUMBER\_$current_time
#RAWDATANAME=$RAWDATANAME1.raw

#echo "Decompressed raw data"
#lz4 -d /home/o2flp/alice/data_raw/$RAWDATANAME1 /home/o2flp/alice/data_raw/$RAWDATANAME

echo "Possible digit file name ? $ROOTNAMEDEF  $ROOTNAMEDEFROF" 

if [ "$BOOLCOMPLETE" = 1 ]; then
    echo "1 "
	root.exe -l -b -q macro/run_rawdecoding_mft.C+\(\"/home/o2flp/alice/data_raw/$RAWDATANAME\",\"/home/o2flp/alice/output_raw/$ROOTNAMEDEF.root\",1\) > $ROOTNAME\_$current_time.out
fi
if [[ "$BOOLCOMPLETE" = 0 ]]; then
    echo " 0 "

	 root.exe -l -b -q macro/run_rawdecoding_mft.C+\(\"/home/o2flp/alice/data_raw/$RAWDATANAME\",\"/home/o2flp/alice/output_raw/$ROOTNAMEDEFROF.root\",0,$ROFNUMBER\) > $ROOTNAME\_ROF$ROFNUMBER\_$current_time.out
fi

echo "Done!!!"
echo "Let's see the digits"
root.exe -l -b -q macro/mapping.C+ | tee mapping\_$current_time.out

echo "Chips name analysis done!!!"
if [ "$BOOLCOMPLETE" = 1 ]; then
    root -l -b -q macro/RawDec.C\(\"/home/o2flp/alice/output_raw/$ROOTNAMEDEF.root\",$VPULSEL\)
fi
if [ "$BOOLCOMPLETE" = 0 ]; then
    root -l -q macro/RawDec.C\(\"/home/o2flp/alice/output_raw/$ROOTNAMEDEFROF.root\",$VPULSEL\)
fi

DIGITFILE=digit_coordinates_$VPULSEL.txt

echo "Creation de txt file $DIGITFILE"

mv digit_coordinate.txt macro/$DIGITFILE
mv macro/$DIGITFILE Thr/$DIGITFILE

rm /home/o2flp/alice/output_raw/$ROOTNAMEDEFROF.root
rm *.out


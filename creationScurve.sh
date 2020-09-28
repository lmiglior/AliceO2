#!/bin/bash
DATARAW=$1
#current_time=$(date "+%y%m%d_%H%M%S")
current_time=$(date "+%Y_%m_%d__%H_%M_%S")
starttime=$(date +"%H.%M.%S,%3N")
echo "Starting time $starttime"
root -l -q creationScurve.C\(\"$DATARAW\",\"$current_time\"\)
finishtime=$(date +"%H.%M.%S,%3N")

echo "Done at time $finishtime"

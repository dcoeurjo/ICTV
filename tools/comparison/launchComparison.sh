#!/bin/zsh
# ./launchMe.sh <Raw file in RAW_PATH folder> <size> <texture_size> <radius_size>
# Example : ./launchMe.sh cube18.raw 18 32 5

ROOT_PATH=$PWD/../..
TOOLS_PATH=$PWD
RAW_PATH=$ROOT_PATH/data/raw
VIZO_PATH=$ROOT_PATH/src/gkit
DGTALTOOLS_BUILD_PATH=~/Dev/DGtalTools/build
RADIUS=$4

cd $VIZO_PATH
./vizo $RAW_PATH/$1 1 $2

cd $TOOLS_PATH
$DGTALTOOLS_BUILD_PATH/visualisation/3dCurvatureViewer -i $RAW_PATH/$1 -r $4 -t $2 -u 255 --exportDat CPU_mean.txt -m mean
$DGTALTOOLS_BUILD_PATH/visualisation/3dCurvatureViewer -i $RAW_PATH/$1 -r $4 -t $2 -u 255 --exportDat CPU_k1.txt -m k1
$DGTALTOOLS_BUILD_PATH/visualisation/3dCurvatureViewer -i $RAW_PATH/$1 -r $4 -t $2 -u 255 --exportDat CPU_k2.txt -m k2

./joinFiles export0_$1:r_r$4.00_cpu.txt CPU_mean.txt CPU_k1.txt CPU_k2.txt
rm -f CPU_mean.txt
rm -f CPU_k1.txt
rm -f CPU_k2.txt
./visuPoints $VIZO_PATH/export0_$1:r_r$4.00_s$3_l0.txt export0_$1:r_r$4.00_cpu.txt
./comparison $VIZO_PATH/export0_$1:r_r$4.00_s$3_l0.txt export0_$1:r_r$4.00_cpu.txt

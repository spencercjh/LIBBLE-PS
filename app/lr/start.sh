#!/usr/bin/env sh

#BSUB -e /data/home/xjsjleiyongmei/cjh/LIBBLE-PS-zq4000/app/lr/result/12.err
#BSUB -o /data/home/xjsjleiyongmei/cjh/LIBBLE-PS-zq4000/app/lr/result/12.out
#BSUB -n 129
#BSUB -q priority
#BSUB -J ps_12
#BSUB -R "span[ptile=16]"
#BSUB –x

ncpus=`cat $LSB_DJOB_HOSTFILE | wc -l `

current=$(date "+%Y_%m_%d_%H%M%S")

EXE=./lr
NUM_SERVER=64
NUM_WORKER=64
EPOCHES=1
MAX_ITERATOR=100

ALL_PROCESSES=$((NUM_WORKER + NUM_SERVER + 1))

TRAIN_DATA_FEATURES=3231961
TRAIN_DATA_ROWS=2396130
TRAIN_DATA_FILE=/data/home/xjsjleiyongmei/dataset/url/train_data
TEST_DATA_FILE=null
TRAIN_DATA_PARTITION=/data/home/xjsjleiyongmei/dataset/url/64

RATE=0.1
LAMBDA=0.0001
PARAMETER_INIT=0

mpirun -machine $LSB_DJOB_HOSTFILE -n $ALL_PROCESSES \
  $EXE \
  -n_servers $NUM_SERVER \
  -n_workers $NUM_WORKER \
  -n_epoches $EPOCHES \
  -n_iters $MAX_ITERATOR \
  -n_cols $TRAIN_DATA_FEATURES \
  -n_rows $TRAIN_DATA_ROWS \
  -test_data_file $TEST_DATA_FILE \
  -train_data_file $TRAIN_DATA_FILE \
  -partition_directory $TRAIN_DATA_PARTITION \
  -rate $RATE \
  -lambda $LAMBDA \
  -param_init $PARAMETER_INIT >lr_"$current".log


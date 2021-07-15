#!/usr/bin/env sh

current=$(date "+%Y_%m_%d_%H%M%S")

EXE=/data/home/xjsjleiyongmei/cjh/LIBBLE-PS-main/app/lr/lr
NUM_SERVER=2
NUM_WORKER=8
EPOCHES=1
MAX_ITERATOR=100

ALL_PROCESSES=$((NUM_WORKER + NUM_SERVER + 1))
NUM_NODE=$((ALL_PROCESSES / 16 + 1))

TRAIN_DATA_FEATURES=47236
TRAIN_DATA_ROWS=20242
TRAIN_DATA_FILE=/data/home/xjsjleiyongmei/dataset/rcv1/train_data
TEST_DATA_FILE=/data/home/xjsjleiyongmei/dataset/rcv1/test_data
TRAIN_DATA_PARTITION=/data/home/xjsjleiyongmei/dataset/rcv1/8

RATE=0.1
LAMBDA=0.0001
PARAMETER_INIT=0

bsub –R "span[ptile=$NUM_NODE]" \
  -J lr_$currentTimeStamp \
  -e lr_${currentTimeStamp}_error.log \
  mpirun -n $((NUM_WORKER + NUM_SERVER + 1)) \
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

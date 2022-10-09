#!/bin/bash

case $1 in
  NO_ACCEL)
  binary="no_accel_picture_kmeans"
  ;;
  THREADED)
  binary="threaded_picture_kmeans"
  ;;
  GPU)
  binary="gpu_accel_picture_kmeans"
  ;;
  *)
  echo "Usage: ./run.sh <NO_ACCEL|THREADED|GPU> \"-f <path_to_file>\""
  exit -1
  ;;
esac

mkdir -p build
cd build
cmake .. -DACCELERATION_TYPE:STRING=$1
make
./$binary $2

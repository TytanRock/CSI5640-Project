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
  echo "Specify NO_ACCEL, THREADED, or GPU in the first parameter"
  exit -1
  ;;
esac

mkdir -p build
cd build
cmake .. -DACCELERATION_TYPE:STRING=$1
make
./$binary $2

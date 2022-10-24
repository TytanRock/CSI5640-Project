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
  echo "Usage: ./run.sh <NO_ACCEL|THREADED|GPU> \"-f <path_to_file> -c <center_count>\""
  exit -1
  ;;
esac

if [ "$3" = "Profile" ]
then
  profile_status="ENABLED"
else
  profile_status="DISABLED"
fi

mkdir -p build
cd build
cmake .. -DACCELERATION_TYPE:STRING=$1 -DPROFILING_STATUS:STRING=$profile_status
make
./$binary $2

if [ "$3" = "Profile" ]
then
  gprof $binary > profile_output.txt
fi

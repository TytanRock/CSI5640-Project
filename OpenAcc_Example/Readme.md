# Compile
CPU:  
$ cd single_gpu
$ pgcc -O3 acc_heat_single.c -o heat

multi-core CPU:  
$ cd single_gpu
$ pgcc -O3 -acc -ta=multicore -Minfo=accel acc_heat_single.c -o heat

GPU:  
$ cd single_gpu
$ pgcc -O3 -acc -ta=tesla:cc35,nofma -Minfo=accel acc_heat_single.c -o heat


Source: https://github.com/rmfarber/ParallelProgrammingWithOpenACC/tree/master/Chapter08/2D-Heat

# Kmeans source

Source comes from https://github.com/pramsey/kmeans


# Building
Project is managed with CMake, so in order to build you need to do the following

## Setup
Follow the instructions in the root readme to install the NVidia toolchain on the computer.

**Note:** Make sure you install the toolchain to your home directory under pgccSDK. CMake assumes the compiler is available under `~/pgccSDK/Linux_x86_64/22.7/compilers/bin/`.

## Building
To build perform the following steps in this directory:
1. `mkdir build`
2. `cd build`
3. `cmake .. -DACCELERATION_TYPE:STRING=<type>` with `<type>` being replaced by one of the following values
   - NO_ACCEL - Do not do any acceleration. This is equivalent to `gcc -O3 -o no_accel_picture_kmeans`
   - THREADED - Perform threaded acceleration. This is equivalent to `pgcc -O3 -acc -ta=multicore -Minfo=accel -o threaded_picture_kmeans`
   - GPU - Perform GPU acceleration. This is equivalent to `pgcc -O3 -acc -ta=tesla:cc35,nofma -Minfo=accel -o gpu_accel_picture_kmeans`
4. `make`
5. Execute the binary you created. The resultant binary will change based on the type of acceleration you provided.

### Profiling
To profile, we are using the gprof tool. To use, simply add `-pg` as a compiler flag. Once you compile, you should see a file called "gmon.out".  
Then run the progam as normal. It will take longer to run with this flag added.  
Finally, run `gprof exectuableName > output.txt`  
This will produce a file that shows what functions were called, how long they took, etc. More info on gprof can be seen at [GPROF Info](https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_chapter/gprof_5.html)

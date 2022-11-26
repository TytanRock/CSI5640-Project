# CSI 5640 Project

Project is to implement kmeans and hardware-accelerate with openacc  
Documents link: 
https://drive.google.com/drive/u/1/folders/0AGVr6qJfNhe8Uk9PVA

## Directory Structure  
| Folder      | Description |
| ----------- | ----------- |
| KMeans/      | Final Project Code |
| OpenAcc_Example/   | Example code to test using OpenACC |
## OpenAcc setup
Connect to VPN (request SECS account if you don't have one at: https://elara.secs.oakland.edu/SECSNetworkLogins/request.aspx). It only takes a few minutes for the to approve. 

ssh into:
yoko.secs.oakland.edu

Installation (on yoko):
- wget https://developer.download.nvidia.com/hpc-sdk/22.7/nvhpc_2022_227_Linux_x86_64_cuda_multi.tar.gz
- tar xpzf nvhpc_2022_227_Linux_x86_64_cuda_multi.tar.gz
- nvhpc_2022_227_Linux_x86_64_cuda_multi/install
- choose option 1 (standalone)
- Install script will ask for a path. You should put your home directory ex)/SECS/home/k/kzunich/pgccSDK
- Installtion should complete and tell you to EXPORT X path, etc. You don't have to do these steps. 

This sdk installs the PGCC compiler. I could not figure out how to setup the paths correctly, so to use it, 
you can't just type pgcc. 
You have to use the full path. In my case:
/SECS/home/k/kzunich/hpc-sdk-build/Linux_x86_64/22.7/compilers/bin/pgcc

Exmaple compile command for single CPU:
/SECS/home/k/kzunich/hpc-sdk-build/Linux_x86_64/22.7/compilers/bin/pgcc -O3 acc_heat_single.c -o heat

pgcc:               Compiler  
-acc:               enables openacc  
-ta=tesla:cc35:     targets the tesla nvidia gpus  
,nofma:             still looking into the meaning of this flag  
-Minfo=accel:       More debug info  
acc_heat_single.c:  program name  
-o heat:             executable output name  

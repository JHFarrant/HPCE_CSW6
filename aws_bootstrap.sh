#!/bin/bash

# aws setup information + # GPU stuff
# @ https://github.com/HPCE/hpce_2014_cw4_df611/blob/master/aws_setup.md


# Download package lists
sudo apt-get update

sudo apt-get -y install g++
sudo apt-get -y install bc
sudo apt-get -y install make


#cd ~/
# install tbb

wget --no-check-certificate 'https://www.threadingbuildingblocks.org/sites/default/files/software_releases/linux/tbb43_20150209oss_lin.tgz'
tar xzvf  tbb43_20150209oss_lin.tgz
#cd tbb43_20150209oss/

# set library path
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:tbb43_20150209oss/lib/intel64/gcc4.4"

rm -rf *.tgz

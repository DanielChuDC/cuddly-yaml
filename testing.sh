#!/bin/bash

# create directory and files
[ -d /opt/test1 ] || sudo mkdir /opt/test1
[ -d /opt/test2 ] || sudo mkdir /opt/test2
[ -d /opt/test3 ] || sudo mkdir /opt/test3
sudo touch /opt/test1/abc{0001..00010}.c
sudo touch /opt/test2/def{0001..00011}.c
sudo touch /opt/test2/def{0001..0004}.c
# sudo cp -r ~/Documents/inkscape/* /opt/test3
#!/bin/sh

gcc packer.c stb_impl.c -lm -o packer
cp packer ~/bin/
cp packer.sh ~/bin/


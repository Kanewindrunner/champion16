#!/bin/bash
#ifndef dir
dir='../traces/traces/*'
#endif
for file in $dir
do
     ./predictor $file >> log
done

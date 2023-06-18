#!/bin/bash

# Default value
SIZEMATRIX=64

while getopts ":s:" opt; do
  case $opt in
    s)
      SIZEMATRIX=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

# Clean and build the project
make clean all run platform=gvsoc VALUE=$SIZEMATRIX




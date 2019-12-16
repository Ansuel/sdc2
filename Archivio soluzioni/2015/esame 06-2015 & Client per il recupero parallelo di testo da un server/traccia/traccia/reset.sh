#!/bin/bash

# remove all FIFOs
find . -type p -delete

# remove output.txt if exists
OUTPUT_FILE="output.txt"
if [ -f $OUTPUT_FILE ]; then
    rm output.txt
fi

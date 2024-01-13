#!/bin/bash

# Compile alert.c
gcc alert.c report_email_sender.c -o alert -lcurl -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running alert..."
    # Run the compiled program
    ./alert
else
    echo "Compilation failed. Please check for errors."
fi


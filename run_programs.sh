#!/bin/bash

# Change to the script's directory
cd "$(dirname "$0")"

# Run the first executable
./fetchData

# Wait for 10 seconds (adjust if needed)
sleep 10

# Run the second executable
./generateReport

# Wait for another 10 seconds
sleep 10

# Run the third executable
./sendForcastReport


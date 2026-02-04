#!/bin/bash

# Function to clean up specific children
cleanup() {
    # Immediately disable the trap to prevent the infinite loop
    trap - SIGINT SIGTERM EXIT
    
    echo -e "\nStopping all processes..."
    
    # Kill the helpers if they are running
    # We use -1 to check if the variable is set
    kill ${PID1} ${PID2} ${SERVER_PID} 2>/dev/null
    
    wait ${PID1} ${PID2} ${SERVER_PID} 2>/dev/null
    exit 0
}

# Trap signals
trap cleanup SIGINT SIGTERM EXIT

while true; do
    echo "--- Starting new session ---"

    python3 alice.py &
    PID1=$!
    
    python3 bob.py &
    PID2=$!

    # Run the server in the BACKGROUND so we can capture its PID
    ./server &
    SERVER_PID=$!

    # Wait for the server to exit (this is the blocking part)
    wait $SERVER_PID
    
    echo "Server crashed with exit code $?. Restarting in 2 seconds..."

    # Kill helpers specifically so we can restart fresh
    kill $PID1 $PID2 2>/dev/null
    wait $PID1 $PID2 2>/dev/null
    
    sleep 2
done
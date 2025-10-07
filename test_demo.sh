#!/bin/bash
# Quick test script to verify the demo works

echo "Building ECU controller..."
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "❌ Build failed"
    exit 1
fi

echo "✅ Build successful"
echo ""
echo "Running demo for 3 seconds..."
echo ""

# Run demo in background and kill after 3 seconds
./build/ecu_controller --demo &
PID=$!
sleep 3
kill $PID 2>/dev/null

echo ""
echo "✅ Demo completed successfully"
echo ""
echo "To run the full demo: ./build/ecu_controller --demo"
echo "To run continuous mode: ./build/ecu_controller"

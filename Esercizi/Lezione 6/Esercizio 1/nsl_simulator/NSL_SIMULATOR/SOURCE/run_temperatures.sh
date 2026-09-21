#!/bin/bash

# Temperatures to simulate
TEMPERATURES=(0.5 0.6 0.7 0.8 0.9 1.0 1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2.0)

# Output directory for the temperature scan
RESULTS_DIR="../OUTPUT/TEMPERATURES"

# Input file
INPUT_FILE="../INPUT/input.dat"

echo "========================================"
echo "Compiling the simulator"
echo "========================================"

# Compile the simulator
make

# Stop if compilation failed
if [ $? -ne 0 ]; then
    echo "Compilation failed. Simulation aborted."
    exit 1
fi

echo "Compilation completed successfully."
echo ""

# Create the results directory
mkdir -p "$RESULTS_DIR"

# Run both Metropolis (2) and Gibbs (3)
for TYPE in 2 3; do

    if [ "$TYPE" -eq 2 ]; then
        ALGORITHM="METROPOLIS"
    else
        ALGORITHM="GIBBS"
    fi

    echo "========================================"
    echo "Running $ALGORITHM simulations"
    echo "========================================"

    # Create algorithm directory
    mkdir -p "$RESULTS_DIR/$ALGORITHM"

    for T in "${TEMPERATURES[@]}"; do

        echo ""
        echo "----------------------------------------"
        echo "$ALGORITHM - T = $T"
        echo "----------------------------------------"

        # Modify simulation type
        sed -i -E \
            "s/^SIMULATION_TYPE[[:space:]]+.*/SIMULATION_TYPE        $TYPE  1.0  0.0/" \
            "$INPUT_FILE"

        # Modify temperature
        sed -i -E \
            "s/^TEMP[[:space:]]+.*/TEMP                   $T/" \
            "$INPUT_FILE"

        # Force a fresh initialization
        sed -i -E \
            "s/^RESTART[[:space:]]+.*/RESTART                0/" \
            "$INPUT_FILE"

        # Remove output files from the previous run
        rm -f ../OUTPUT/total_energy.dat
        rm -f ../OUTPUT/magnetization.dat
        rm -f ../OUTPUT/specific_heat.dat
        rm -f ../OUTPUT/susceptibility.dat
        rm -f ../OUTPUT/acceptance.dat

        # Run the simulation
        ./simulator.exe > /dev/null

        # Directory for this temperature
        TEMP_DIR="$RESULTS_DIR/$ALGORITHM/T_$T"
        mkdir -p "$TEMP_DIR"

        # Save output files
        cp ../OUTPUT/total_energy.dat "$TEMP_DIR/"
        cp ../OUTPUT/magnetization.dat "$TEMP_DIR/"
        cp ../OUTPUT/specific_heat.dat "$TEMP_DIR/"
        cp ../OUTPUT/susceptibility.dat "$TEMP_DIR/"
        cp ../OUTPUT/acceptance.dat "$TEMP_DIR/"

        echo "Results saved in $TEMP_DIR"

    done
done

echo ""
echo "========================================"
echo "All simulations completed."
echo "========================================"
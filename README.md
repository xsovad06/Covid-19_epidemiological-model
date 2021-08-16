# Covid-19 epidemiological model
 epidemiological model using a cellular automata.
 Simulating spread of covid-19.

# Build
Fist of all you need to build progam with command `make`. 

# Run
There are two options for running the progam:
    `./automat <max_days> <init_infected_num> <trips_per_day> <trip_max_distance>`
    `make run` -but with predefined arguments from makefile

# Clean
    `make clean`              remove files ".o"

# Program parameters
    `<max_days>`              Max days of simulation
    `<init_infected_num>`     Initial number of infected
    `<trips_per_day>`         Number of trips per day of one cell
    `<trip_max_distance>`     Max distance of one trip

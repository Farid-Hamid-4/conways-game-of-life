# Conways Game of Life

Conways game of life as a command line interface implemented in C using both data-parallel and task-parallel methods.

## Acknowledgements
A University of Guelph CIS*3090 Parallel Programming assignment

## Prerequisites
- gcc
- make
- pthread

## Usage
- Run the `make` command in the terminal from the root folder
- For a data parallel test use `./gol_data n_threads grid_size n_iterations`
- For a task parallel test use `./gol_task grid_size n_iterations`

## Examples
**Data Parallel**

    ./gol_data 3 100 10000

**Task Parallel**

    ./gol_task 100 10000

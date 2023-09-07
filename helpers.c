#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "helpers.h"

/*
 * Validate command line arguments for gol_data, data parallelism. Report usage for failures
 */
int validateDataCmdArgs (int argc, char **argv) {
    if ((argc < 4) || (argc > 5)) {
        printf("usage: /gol_data nThreads gridSize nIteratiosn -d\n");
        exit(1);
    }
    else if (argc == 5) {
        if ((argv[4][0] != '-') || (argv[4][1] != 'd')) {
            printf("usage: /gol_data nThreads gridSize nIterations -d\n");
            exit(1);
        }
        else return 1; // Return 1 to display grid
    }
    return 0; // Return 0 to not display grid
}

/*
 * Validate command line arguments for gol_task, task parallelism. Report usage for failures
 */
int validateTaskCmdArgs (int argc, char **argv) {
    if ((argc < 3) || (argc > 4)) {
        printf("usage: /gol_data gridSize nIteratiosn -d\n");
        exit(1);
    }
    else if (argc == 4) {
        if ((argv[3][0] != '-') || (argv[3][1] != 'd')) {
            printf("usage: /gol_data gridSize nIterations -d\n");
            exit(1);
        }
        else return 1; // Return 1 to display grid
    }
    return 0; // Return 0 to not display grid
}

/*
 * Create an empty grid. Allocate memory for a 2D array
 */
int **createGrid (int size) {
    int **grid = (int**)malloc(size * sizeof(int*));
    for(int i = 0; i < size; i++) {
        grid[i] = (int*)malloc(size * sizeof(int));
    }
    return grid;
}

/*
 * Set the current grid cell values to the next grid cell values
 */
void swapGrid (int **current_grid, int **next_grid, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            current_grid[i][j] = next_grid[i][j];
        }
    }
}

/*
 * Fill the grid with 1's for live cells and 0's for dead cells
 */
int **randomizeGrid (int **grid, int size) {
    srand(time(0));
    for(int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            grid[i][j] = rand() % 2;
        }
    }
    return grid;
}

/*
* Counts the number of live neighbors for any given cell and returns its value 
*/
int countLiveNeighbors (int **grid, int size, int rowIndex, int columnIndex) {
    int i, j, live_neighbors = 0;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            // Check if the surrounding cells are within the boundaries of the grid
            if (((rowIndex + i ) >= size) || ((columnIndex + j) >= size) || ((rowIndex + i) < 0) || ((columnIndex + j) < 0)) {
                continue;
            }
            // If the neighboring cell is alive, increment live neighbors
            if (grid[rowIndex + i][columnIndex + j] == 1) {
                live_neighbors++;
            }
        }
    }
    // If the value of the cell passed in to the function is 1, decrement live neighbors to ensure it doesn't count it
    if (grid[rowIndex][columnIndex] == 1) {
        live_neighbors--;
    }
    return live_neighbors;
}

/*
 * Use Conways game of life rules to determine which cells live/die/stay dead
 */
int nextGeneration (int live_neighbors, int **current_grid, int i, int j) {
    if (current_grid[i][j] == 1) {
        if (live_neighbors < 2 || live_neighbors > 3) return 0; // Kill a live cell
        else if (live_neighbors == 2 || live_neighbors == 3) return 1; // Keep alive to next generation
    }
    else if (current_grid[i][j] == 0) {
        if (live_neighbors == 3) return 2; // Bring a dead cell to life
    }
    return 4;// Keep a dead cell dead
}

/*
 * Display the grids
 */
void displayGrid(int **grid, int size) {
    for(int i = 0; i < size + 2; i++) printf("-");
    printf("\n");

    for(int i = 0; i < size ; i++) {
        printf("|");
        for(int j = 0; j < size; j++) {
            if(grid[i][j] == 1)
                printf("x");
            else printf(" ");
        }
        printf("|\n");
    }

    for(int i = 0; i < size + 2; i++) printf("-");
    printf("\n");
}

/*
 *  Each thread computes its own set of cells, calculates the number of live neighbors, calls nextGeneration to write to the next_grid based on CGOL rules
 */
void updateGrid (int **current_grid, int **next_grid, int grid_size, int n_threads, void* arg) {
    int cell_num = 0;
    long thread_num = (long)arg;
    int live_neighbors;
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            cell_num++;
            /*
             * The remainder of cell_num % n_threads determines whether the thread passed in is allowed to operate on the given cell.
             * e.g. thread_num = 1, cell_num = 1 and n_threads = 3
             * 1 % 3 = 1 == thread_num, so thread 1 will operate on cell i:0 j:1
             */ 
            if(cell_num % n_threads == thread_num) {
                live_neighbors = countLiveNeighbors(current_grid, grid_size, i, j);
                if (nextGeneration(live_neighbors, current_grid, i, j) == 0 || nextGeneration(live_neighbors, current_grid, i, j) == 3) {
                    next_grid[i][j] = 0;
                }
                else if (nextGeneration(live_neighbors, current_grid, i, j) == 1 || nextGeneration(live_neighbors, current_grid, i, j) == 2) {
                    next_grid[i][j] = 1;
                }
            }
        }
    }
}

/*
 * Free the grid memory
 */
void **deleteGrid(int **grid, int size) {
    for(int i = 0; i < size; i++)
        free(grid[i]);
    free(grid);
    return 0;
}


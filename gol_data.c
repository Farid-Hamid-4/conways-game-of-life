#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "helpers.h"

/* Global variables */
void *routine(void *arg);
int flag_display_grid;
int n_threads;
int grid_size;
int n_iterations;
int **current_grid;
int **next_grid;
int barrier_thread_count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond; 

int main (int argc, char **argv) {

    clock_t start, end;
    double cpu_time_used;
    // Set flag_display_grid to 1 if cmd line had -d, otherwise set it to 0
    flag_display_grid = validateDataCmdArgs(argc, argv);

    // Store values of the cmd line args
    n_threads = atoi(argv[1]);
    grid_size = atoi(argv[2]);
    n_iterations = atoi(argv[3]);

    // Create the current and next grid. Randomize the cell values for current grid
    current_grid = createGrid(grid_size);
    next_grid = createGrid(grid_size);
    randomizeGrid(current_grid, grid_size);
    
    // Initialize mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Create pthreads and join at the end
    pthread_t tid[n_threads];
    int creation_error;
    start = clock();
    for (long thread_num = 0; thread_num < n_threads; thread_num++) {
        creation_error = pthread_create(&tid[thread_num], NULL, routine, (void*)thread_num);
        if (creation_error != 0) {
            printf("Cannot create thread, error: %d", creation_error);
            exit(-1);
        }
    }
    for (int i = 0; i < n_threads; i++) {
        pthread_join(tid[i], NULL);
    }
    /* Print the last iteration of the board. routine function, prints the intial board up to the second last iteration of the board */
    if(flag_display_grid == 1) {
        swapGrid(current_grid, next_grid, grid_size);
        displayGrid(current_grid, grid_size);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Clock time: %lf seconds\n", cpu_time_used);

    // Destroy and free memory
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    deleteGrid(current_grid, grid_size);
    deleteGrid(next_grid, grid_size);

    return 0;
}

/* Thread function */
void *routine (void *arg) {
    for (int n = 0; n < n_iterations; n++) {
        pthread_mutex_lock(&mutex);
        barrier_thread_count++;
        if (barrier_thread_count == n_threads) {
            // Don't swap the initials grid with the next grid in the 0th iteration
            if (n != 0) {
                swapGrid(current_grid, next_grid, grid_size);
            }
            if (flag_display_grid == 1) {
                displayGrid(current_grid, grid_size);
            }
            barrier_thread_count = 0;
            pthread_cond_broadcast(&cond);
        }
        else {
            while (pthread_cond_wait(&cond, &mutex) != 0);
        }
        // Update the values in next grid based on current
        updateGrid(current_grid, next_grid, grid_size, n_threads, arg);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
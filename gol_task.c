#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "helpers.h"
#include "queue.h"

#define NTHREADS 3

void *countNeighbors(void *arg);
void *writeLiveCells(void *arg);
void *writeEmptyCells(void *arg);
Queue live_queue;
Queue empty_queue;
int **current_grid;
int **next_grid;
int flag_display_grid;
int grid_size;
int n_iterations;
int counter = 0;
pthread_mutex_t live_mutex, empty_mutex;

int main(int argc, char *argv[]) {
    clock_t start, end;
    double cpu_time_used;

    flag_display_grid = validateTaskCmdArgs(argc, argv);

    grid_size = atoi(argv[1]);
    n_iterations = atoi(argv[2]);

    current_grid = createGrid(grid_size);
    next_grid = createGrid(grid_size);
    randomizeGrid(current_grid, grid_size);

    initializeQueue(&live_queue);
    initializeQueue(&empty_queue);

    pthread_mutex_init(&live_mutex, NULL);
    pthread_mutex_init(&empty_mutex, NULL);

    pthread_t tid[NTHREADS];
    start = clock();
    pthread_create(&tid[0], NULL, countNeighbors, NULL);
    pthread_create(&tid[1], NULL, writeLiveCells, NULL);
    pthread_create(&tid[2], NULL, writeEmptyCells, NULL);
    for (int i = 0; i < NTHREADS; i++) pthread_join(tid[i], NULL);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Clock time: %lf seconds\n", cpu_time_used);

    pthread_mutex_destroy(&live_mutex);
    pthread_mutex_destroy(&empty_mutex);
    deleteGrid(current_grid, grid_size);
    deleteGrid(next_grid, grid_size);

    return 0;
}

void *countNeighbors(void *arg) {
    int live_neighbors;
    for(int n = 0; n < n_iterations; n++) {
        for(int i = 0; i < grid_size; i++) {
            for(int j = 0; j < grid_size; j++) {
                live_neighbors = countLiveNeighbors(current_grid, grid_size, i, j);
                if (nextGeneration(live_neighbors, current_grid, i, j) == 0 || nextGeneration(live_neighbors, current_grid, i, j) == 4) {
                    pthread_mutex_lock(&empty_mutex);
                    enqueue(&empty_queue, i, j);
                    pthread_mutex_unlock(&empty_mutex);
                }
                else if (nextGeneration(live_neighbors, current_grid, i, j) == 1 || nextGeneration(live_neighbors, current_grid, i, j) == 2) {
                    pthread_mutex_lock(&live_mutex);
                    enqueue(&live_queue, i, j);
                    pthread_mutex_unlock(&live_mutex);
                }
            }
        }
        while((&live_queue)->head != NULL || (&empty_queue)->head != NULL);
        if(n != 0) {
            swapGrid(current_grid, next_grid, grid_size);
        }
        if(flag_display_grid == 1) {
            displayGrid(current_grid, grid_size);
        }
    }
    counter = 1;
    return NULL;
}

void *writeLiveCells(void *arg) {
    int i, j;
    while(counter == 0) {
        if ((&live_queue)->head != NULL){
            int *node;
            pthread_mutex_lock(&live_mutex);
            while((node = dequeue(&live_queue)) != NULL) {
                i = node[0];
                j = node[1];
                next_grid[i][j] = 1;
                free(node);
            }
            pthread_mutex_unlock(&live_mutex);
        }
    }
    return NULL;
}

void *writeEmptyCells(void *arg) {
    int i, j;
    while (counter == 0) {
        if((&empty_queue)->head != NULL) {
            int *node;
            pthread_mutex_lock(&empty_mutex);
            while ((node = dequeue(&empty_queue)) != NULL) {
                i = node[0];
                j = node[1];
                next_grid[i][j] = 0;
                free(node);
            }
            pthread_mutex_unlock(&empty_mutex);
        }
    }
    return NULL;
}

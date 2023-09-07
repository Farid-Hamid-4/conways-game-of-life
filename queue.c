#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"

void initializeQueue(Queue *queue) {
    queue->head = NULL;
    queue->tail = NULL;
}

int enqueue(Queue *queue, int i, int j) {
    Node *node = malloc(sizeof(Node));
    if (node == NULL)
        return 0;

    node->row_index = i;
    node->column_index = j;
    node->next = NULL;

    if (queue->tail != NULL)
        queue->tail->next = node;
    queue->tail = node;

    if (queue->head == NULL)
        queue->head = node;

    return 1;
}

int *dequeue(Queue *queue) {
    if (queue->head == NULL)
        return NULL;

    int *result = malloc(2 * sizeof(int));
    result[0] = queue->head->row_index;
    result[1] = queue->head->column_index;
    Node *node = queue->head;
    queue->head = queue->head->next;
    free(node);
    if (queue->head == NULL)
        queue->tail = NULL;

    return result;
}

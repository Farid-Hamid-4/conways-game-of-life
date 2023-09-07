typedef struct node {int row_index; int column_index; struct node *next;} Node;
typedef struct queue {Node *tail; Node *head;} Queue;
void initializeQueue(Queue *queue);
int enqueue(Queue *queue, int i, int j);
int *dequeue(Queue *queue);
int isEmpty(Queue *queue);

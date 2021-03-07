#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_STR_LEN 16

pthread_mutex_t queueAccess;
pthread_t tid; 

struct node *QueueHead = NULL;
struct node *QueueTail = NULL;

struct node {
    uint16_t     student_number;
    char         student_name[MAX_NAME_STR_LEN];
    struct node *next;
};

/**
 * check is queue is empty
 *
 * @param 
 * @return bool
 */
bool is_queue_empty (void)
{
    return QueueHead?false:true;
}

/**
 * check is queue and print output
 *
 * @param 
 * @return
 */
void check_queue (void)
{
    if (is_queue_empty())
        printf("\nQueue is empty.\n");
    else
        printf("\nQueue is not empty.\n");
}

/**
 * displays elements in queue
 *
 * @param 
 * @return
 */
void display_queue (void)
{
    if (is_queue_empty())
    {
        printf("Queue is empty.\n");
        return;
    }
       
    struct node* posPtr = QueueHead;
    int i = 1;

    printf("Display current students on queue {\n");
    printf("  Index   Name \t\tNumber\n");
    printf("  ----------------------------\n");
    while (posPtr)
    {
        printf("  %d \t  %s \t%d\n", i, posPtr->student_name, posPtr->student_number);
        posPtr = posPtr->next;
        i++;
    }
    printf("}\n");
}

/**
 * add item to queue
 *
 * @param student_number, student_name
 * @return bool
 */
bool enqueue (uint16_t student_number, char *student_name)
{
    printf("\nEnqueue student: %s (#%d)\n", student_name, student_number);
    bool rc = true;

    struct node* newNode = (struct node*) calloc (1, sizeof(struct node));
    strncpy(newNode->student_name, student_name, MAX_NAME_STR_LEN);
    newNode->student_number = student_number;
    newNode->next = NULL;

    pthread_mutex_lock(&queueAccess);
    if (!QueueHead)
    {
        QueueHead = newNode;
        QueueTail = newNode;
    }
    else
    {
        QueueTail->next = newNode;
        QueueTail = newNode;
    }
    pthread_mutex_unlock(&queueAccess);

    return rc;
}

/**
 * remove item from queue
 *
 * @param
 * @return node*
 */
struct node* dequeue (void)
{
    struct node* rPtr = NULL;

    pthread_mutex_unlock(&queueAccess);
    if (!QueueHead)
        return rPtr;

    if (QueueHead == QueueTail)
        QueueTail = NULL;
    
    rPtr = QueueHead;
    QueueHead = QueueHead->next;
    pthread_mutex_unlock(&queueAccess);

    rPtr->next = NULL; // good memory management habit
    printf("\nDequeue student: %s #%d\n", rPtr->student_name, rPtr->student_number);

    return rPtr;
}

void *test_queue (void* arg) 
{
    printf("\nRun UT..\n");

    const char studentNameList[8][MAX_NAME_STR_LEN] = {"John Daniel","Jane Erin","Abdul Abu","Lisa Edwin","Preeti Aja","Amal Dale","Kwasi Enok","Kabal Smoke"};
    const uint16_t studentNumberList[8] = {101,200,560,778,455,787,998,1504};
    struct node* tempNode = NULL;
    int index = 0;

    // check if queue is empty
    check_queue();

    // testing queues - enqueue (0-4)
    for (index=0; index<5; index++)
    {
        enqueue(studentNumberList[index], (char*)studentNameList[index]);
        display_queue();
    }

    // testing queues - dequeue
    tempNode = dequeue();
    if (tempNode) 
    {
        printf("Dequeued %s\n", tempNode->student_name);
        free(tempNode); // use same memory spot and free after using is caller, saves mem and runs fast
        display_queue();
    }

    // testing queues - enqueue (5-7)
    for (index=5; index<8; index++)
    {
        enqueue(studentNumberList[index], (char*)studentNameList[index]);
        display_queue();
    }

    // testing queues - dequeue all
    while (!is_queue_empty()) 
    {
        tempNode = dequeue();
        printf("Dequeued %s\n", tempNode->student_name);
        free(tempNode); // use same memory spot and free after using is caller, saves mem and runs fast
        display_queue();
    }

    check_queue();

    return NULL;
}

int main (void)
{
    printf("I'm Alive!\n");

    if (pthread_mutex_init(&queueAccess, NULL) != 0) { 
        printf("Mutex initializion failed.\n"); 
        return 1; 
    }
    else   
        printf("Mutex initialized.\n");

    int error = pthread_create(&tid, NULL, &test_queue, NULL);
    if (error != 0) 
        printf("\nThread can't be created :[%s]", strerror(error));
    else
        printf("Thread created.\n");

    pthread_join(tid, NULL); 
    pthread_mutex_destroy(&queueAccess); // good clean up

    return 0;
}

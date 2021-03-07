#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME_STR_LEN 16

pthread_mutex_t queueAccess;
pthread_t tid[3]; 
uint32_t workToDo = 0;

struct node *QueueHead = NULL;
struct node *QueueTail = NULL;

struct node {
    uint32_t     student_number;
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
        printf("Queue is empty (entries %d).\n", workToDo);
    else
        printf("Queue is not empty (entries %d).\n", workToDo);
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
bool enqueue (uint32_t student_number, char *student_name)
{
    bool rc = true;

    struct node* newNode = (struct node*) calloc (1, sizeof(struct node));
    strncpy(newNode->student_name, student_name, MAX_NAME_STR_LEN);
    newNode->student_number = student_number;
    newNode->next = NULL;

    pthread_mutex_lock(&queueAccess);
    printf("  --> enqueue student: %s \t(#%d)\n", student_name, student_number);

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
    workToDo++;
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
    workToDo--;

    rPtr->next = NULL; // good memory management habit
    printf(" <-- dequeue student: %s \t(#%d)\n", rPtr->student_name, rPtr->student_number);
    pthread_mutex_unlock(&queueAccess);

    return rPtr;
}


/**
 * testing/simulating queue - add students to queue
 *
 * @param
 * @return
 */
void *add_students_to_queue_1 (void* arg) 
{
    const char studentNameList[10][MAX_NAME_STR_LEN] = { "John Daniel",
                                                         "Jane Erin",
                                                         "Abdul Abu",
                                                         "Lisa Edwin",
                                                         "Preeti Aja",
                                                         "Amal Dale",
                                                         "Kwasi Enok",
                                                         "Kabal Smoke",
                                                         "Sastoshi Nu",
                                                         "Ece Berat" };
    const uint32_t studentNumberList[10] = { 17051,
                                             25060,
                                             45650,
                                             77781,
                                             46555,
                                             72387,
                                             11998,
                                             81504,
                                             45672,
                                             12121 };
    int index = 0;

    for (index=0; index<5; index++)
        enqueue(studentNumberList[index], (char*)studentNameList[index]);

    sleep(1);

    for (index=5; index<10; index++)
        enqueue(studentNumberList[index], (char*)studentNameList[index]);

    return NULL;
}

/**
 * testing/simulating queue - add students to queue
 *
 * @param
 * @return
 */
void *add_students_to_queue_2 (void* arg) 
{
    const char studentNameList[8][MAX_NAME_STR_LEN] = { "Liam Miller",
                                                        "Fiona Edward",
                                                        "Obinna Igwe",
                                                        "Omar Omar",
                                                        "Ibrahim Abu",
                                                        "Ying Yang",
                                                        "Tule Diwa",
                                                        "Lara Croft"};
    const uint32_t studentNumberList[8] = { 11201,
                                            26500,
                                            45560,
                                            70178,
                                            25455,
                                            11787,
                                            32998,
                                            61504 };
    int index = 0;

    for (index=0; index<2; index++)
        enqueue(studentNumberList[index], (char*)studentNameList[index]);

    sleep(1);

    for (index=2; index<4; index++)
        enqueue(studentNumberList[index], (char*)studentNameList[index]);
    
    sleep(1);

    for (index=4; index<6; index++)
        enqueue(studentNumberList[index], (char*)studentNameList[index]);

    sleep(1);

    for (index=6; index<8; index++)
        enqueue(studentNumberList[index], (char*)studentNameList[index]);

    return NULL;
}

/**
 * testing/simulating - process students on queue
 *
 * @param
 * @return
 */
void *process_students_on_queue (void* arg) 
{
    struct node* tempNode = NULL;
    
    while (true)
    {
        while (!is_queue_empty()) 
        {
            tempNode = dequeue();
            usleep(100); // do work / process student
            free(tempNode); // use same memory spot and free after using is caller
        }
    }

    return NULL;
}

int main (void)
{
    printf("I'm Alive!\n");
    int error;

    if (pthread_mutex_init(&queueAccess, NULL) != 0) { 
        printf("Mutex initializion failed.\n"); 
        return 1; 
    }
    else   
        printf("Mutex initialized.\n");

    error = pthread_create(&tid[0], NULL, &add_students_to_queue_1, NULL);
    if (error != 0) 
        printf("\nThread can't be created :[%s]", strerror(error));
    else
        printf("Thread created.\n");

    error = pthread_create(&tid[1], NULL, &add_students_to_queue_2, NULL);
    if (error != 0) 
        printf("\nThread can't be created :[%s]", strerror(error));
    else
        printf("Thread created.\n");

    error = pthread_create(&tid[2], NULL, &process_students_on_queue, NULL);
    if (error != 0) 
        printf("\nThread can't be created :[%s]", strerror(error));
    else
        printf("Thread created.\n");

    check_queue();

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL); 
    pthread_join(tid[2], NULL); 
    pthread_mutex_destroy(&queueAccess); // good clean up

    return 0;
}

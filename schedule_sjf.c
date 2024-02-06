#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>

int taskCount = 0;

// Head and tail nodes for the task queue

struct node *head = NULL;
struct node *tail = NULL;

void add(char *name, int priority, int burst){

    //Memory allocation for a new node

    struct node *newNode = malloc(sizeof(struct node));
    
    //Break execution if memory runs out
    if(newNode == NULL){
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    //Task initialization
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->tid = taskCount;
    newNode->task = newTask;
    newNode->next = NULL;

    //Head and Tail access to the waiting queue
    if(tail == NULL){
        head = newNode;
        tail = newNode;
    } else {

        struct node* prev = NULL;
        struct node* current = head;

        while(current != NULL && current->task->burst < newTask->burst){
            prev = current;
            current = current->next;
        }

        while(current != NULL && current->task->burst == newTask->burst){
            prev = current;
            current = current->next;
        }

        if(prev == NULL){
            newNode->next = head;
            head = newNode;
        } else {
            prev->next = newNode;
            newNode->next = current;
        }

        if(current == NULL){
            tail = newNode;
        }        

    }
    // To keep track of number of tasks for the 
    // avg calculation
   taskCount++;
}

void schedule(){
    struct node *curr = head;
    int currentTime = 0;

    float totalWaitingTime = 0;
    float totalTurnAroundTime = 0;
    float totalResponseTime = 0;

    /*
        Iterate over all the tasks in a SJF manner, 
        maintaining shortest job first   

        In every iteration we also calculate total waiting,
        turnaround, and response times. 
    */
    while(curr != NULL){

        totalWaitingTime += currentTime;
        totalTurnAroundTime += currentTime + curr->task->burst;
        totalResponseTime += currentTime;

        run(curr->task, curr->task->burst);

        currentTime += curr->task->burst;

        /**
        Memory deallocation
        */
        struct node *tofree = curr;
        curr = curr->next;
        free(tofree);
    }
    
   // Find the average against the total number of tasks
    totalWaitingTime /= taskCount;
    totalTurnAroundTime /= taskCount;
    totalResponseTime /= taskCount;

    //Printing the averages
    printf("\n");
    printf("Average Waiting Time: %.2f\n", totalWaitingTime);
    printf("Average Turnaround Time: %.2f\n", totalTurnAroundTime);
    printf("Average Response Time: %.2f\n", totalResponseTime);
}

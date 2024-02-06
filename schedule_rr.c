#include "schedulers.h"
#include "task.h"
#include "cpu.h"

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int taskCount = 0;

typedef struct TaskNode {
    Task task;
    struct TaskNode *next;
    int timeLeft;
} TaskNode;

// Head and tail nodes for the task queue

TaskNode *head = NULL;
TaskNode *tail = NULL;


void add(char *name, int priority, int burst){

    //Memory allocation for a new node
    TaskNode *newNode = malloc(sizeof(TaskNode));
    
    //Break execution if memory runs out
    if(newNode == NULL){
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    //Task initialization
    Task newTask;
    newTask.name = name;
    newTask.priority = priority;
    newTask.burst = burst;
    newNode->task = newTask;
    newNode->next = NULL;
    newNode->timeLeft = burst;

    if(tail == NULL){
        head = newNode;
        tail = newNode;
    } else {
       tail->next = newNode;
       tail = newNode;
    }
    
    // To keep track of number of tasks for the 
    // avg calculation
   taskCount++;
}

void schedule(){
    TaskNode *curr = head;

    int currentTime = 0;
    float totalWaitingTime = 0.0;
    float totalTurnAroundTime = 0.0;
    float totalResponseTime = 0.0;

    /*
        Iterate over all the tasks in a RR manner, 
        switching based on QUANTUM, unless the timeleft
        for a job is less than the quantum.   

        In every iteration we also calculate total waiting,
        turnaround, and response times. 
    */
    while(curr != NULL){
        
        if(curr->task.burst > 0){

            bool outOfTime = curr->task.burst >= QUANTUM;
            int executionTime = (!outOfTime) ? curr->task.burst : QUANTUM;

            if(curr->timeLeft == curr->task.burst){
                totalResponseTime += currentTime;
            }

            run(&curr->task, executionTime);
            curr->task.burst -= executionTime;

            currentTime += executionTime;

            if(curr->task.burst == 0){
                // Task is complete
                totalTurnAroundTime += currentTime;
                totalWaitingTime += currentTime - curr->timeLeft;
            }

            tail->next = curr;
            tail = curr;
            curr = curr->next;
            tail->next = NULL;
        }else{
            curr = curr->next;
        }
    }
    
    /**
        Memory deallocation
    */
    TaskNode *tempNode = head;

    while (tempNode != NULL) {
        TaskNode *toFree = tempNode;
        tempNode = tempNode->next;

        free(toFree);
    }
    
    // Find the average against the total number of tasks
    totalWaitingTime /= taskCount;  
    totalTurnAroundTime /= taskCount;
    totalResponseTime /= taskCount;

    printf("\n");
    printf("Average Waiting Time: %.2f\n", totalWaitingTime);
    printf("Average Turnaround Time: %.2f\n", totalTurnAroundTime);
    printf("Average Response Time: %.2f\n", totalResponseTime);
}


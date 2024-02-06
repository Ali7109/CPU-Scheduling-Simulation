#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>

int taskCount = 0;

//Pointers to the first and last task in the queue
struct node *head = NULL;
struct node *tail = NULL;


void add(char *name, int priority, int burst) {

    //Memory allocation of the new node
    struct node *newNode = malloc(sizeof(struct node));

    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    //  Memory allocation of the new Task
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->tid = taskCount;
    newNode->task = newTask;
    newNode->next = NULL;

    if (tail == NULL) {

        head = newNode;
        tail = newNode;

    } else {

        struct node* prev = NULL;
        struct node* current = head;

        while (current != NULL && current->task->priority > newTask->priority ) {
            prev = current;
            current = current->next;
        }
        while (current != NULL && current->task->priority == newTask->priority ) {
            prev = current;
            current = current->next;
        }
        newNode->next = current;
        if (prev == NULL) {
            head = newNode;
        } else {
            prev->next = newNode;
        }

        if (current == NULL) {
            tail = newNode;
        }
    }

    taskCount++;
}

void schedule() {

    
    struct node *curr = head;
    int currentTime = 0;

    float totalWaitingTime = 0;
    float totalTurnAroundTime = 0;
    float totalResponseTime = 0;

    while (curr != NULL) {

        totalWaitingTime += currentTime;
        totalTurnAroundTime += currentTime + curr->task->burst;
        totalResponseTime += currentTime;

        run(curr->task, curr->task->burst);

        currentTime += curr->task->burst;

        /**
            Memory deallocation
        */
        struct node *temp = head;
        temp = curr;
        curr = curr->next;
        free(temp);
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

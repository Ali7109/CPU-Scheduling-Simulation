#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct TaskNode {
    Task task;
    struct TaskNode *next;
    int timeLeft;
} TaskNode;

TaskNode *head = NULL;
TaskNode *tail = NULL;

int taskCount = 0;

    /*
        Adds in a way to keep priority based order
    */
void add(char *name, int priority, int burst) {
    TaskNode *newNode = malloc(sizeof(TaskNode));
    if (newNode == NULL) {
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

    if (head == NULL) {
        head = newNode;
        tail = newNode;
    } else {
        if (newNode->task.priority > head->task.priority) {
            newNode->next = head;
            head = newNode;
        } else {
            TaskNode *current = head;
            while (current->next != NULL && current->next->task.priority >= newNode->task.priority) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;

            if (newNode->next == NULL) {
                tail = newNode;
            }
        }
    }

    //Keeping track of number of tasks for the avg
    taskCount++;
}

void schedule() {

    int currentTime = 0;
    bool tasksRemaining = true;

    float totalWaitTime = 0.0;
    float totalTurnaroundTime = 0.0;
    float totalResponseTime = 0.0;

    TaskNode *priorityHead = head;

    /*
        Iterate over all the tasks in a priority based manner.

        After a specific priority comes to an end, it will make sure if 
        there are no process remaining in the current priority,
        and if not it will progress.
        In every iteration we also calculate total waiting,
        turnaround, and response times. 
    */
    while (tasksRemaining) {

        tasksRemaining = false;
        TaskNode *curr = priorityHead;

        while (curr != NULL && curr->task.priority == priorityHead->task.priority) {
            if (curr->task.burst > 0) {
                tasksRemaining = true;

                bool finishTask = false;
                if(curr->next == NULL){
                    finishTask = true;
                }
                else if ((curr->next->task.priority != curr->task.priority)
                && 
                (curr == priorityHead)){
                    finishTask = true;
                }


                bool outOfTime = curr->task.burst > QUANTUM;
                int executionTime = !finishTask ? (outOfTime ? QUANTUM : curr->task.burst) : curr->task.burst;

                if (curr->task.burst == curr->timeLeft) {

                    totalResponseTime += currentTime;

                }

                run(&curr->task, executionTime);
                curr->task.burst -= executionTime;

                currentTime += executionTime;

                if (curr->task.burst == 0) {
                    // Task is complete
                    totalWaitTime += currentTime - curr->timeLeft;
                    totalTurnaroundTime += currentTime;

                }
            }
            curr = curr->next;
        }

        
        if(curr != NULL){
            //In this case we are checking if there is a 
            //task which we did not complete
            if(!tasksRemaining){

                priorityHead = curr;
                tasksRemaining = true;

            }

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
    totalWaitTime /= taskCount;  
    totalTurnaroundTime /= taskCount;
    totalResponseTime /= taskCount;

    printf("\n");
    printf("Average Waiting Time: %.2f\n", totalWaitTime);
    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime);
    printf("Average Response Time: %.2f\n", totalResponseTime);
}

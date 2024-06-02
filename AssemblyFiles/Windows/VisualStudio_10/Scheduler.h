#ifndef SCHEDULER_H
#define SCHEDULER_H

struct Process {
    int id;        
    int arrival_time; 
    int burst_time; 
};

void scheduleProcesses(struct Process processes[], int n, int *completion_order);

#endif 
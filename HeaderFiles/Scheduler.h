#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef struct Process {
    int id;        
    int arrival_time; 
    int burst_time; 
	int waiting_time;
	int completion_time;
}Process;

#endif 
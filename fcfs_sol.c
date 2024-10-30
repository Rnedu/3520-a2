/*
    COMP3520 Exercise 7 - FCFS Dispatcher

    usage:

        ./fcfs <TESTFILE>
        where <TESTFILE> is the name of a job list
*/

/* Include files */
#include "fcfs.h"

int main (int argc, char *argv[])
{
    /*** Main function variable declarations ***/

    FILE * input_list_stream = NULL;

    PcbPtr queue_0 = NULL; //custom
    PcbPtr queue_1 = NULL; //custom
    PcbPtr queue_2 = NULL; //custom

    PcbPtr job_queue = NULL;
    PcbPtr current_process = NULL;
    PcbPtr process = NULL;
    PcbPtr temp_process = NULL;

    int t0, t1, t2, W; //custom
    int timer = 0;

    int turnaround_time, response_time;
    double av_turnaround_time = 0.0, av_wait_time = 0.0, av_response_time = 0.0;
    int n = 0;

//  1. Populate the FCFS queue

    if (argc <= 0)
    {
        fprintf(stderr, "FATAL: Bad arguments array\n");
        exit(EXIT_FAILURE);
    }
    else if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <TESTFILE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!(input_list_stream = fopen(argv[1], "r")))
    {
        fprintf(stderr, "ERROR: Could not open \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    while (!feof(input_list_stream)) {  // put processes into job_queue
        process = createnullPcb();
        if (fscanf(input_list_stream,"%d, %d, %d",
             &(process->arrival_time), 
             &(process->service_time),
             &(process->queue_priority)) != 3) {
            free(process);
            continue;
        }
        process->remaining_cpu_time = process->service_time;
        process->status = PCB_INITIALIZED;
        job_queue = enqPcb(job_queue, process);
        n++;
    }

//  1.1 Ask User for time quantum
    printf("Please enter a positive integer for the time quantum t0 for the Level 0 queue: ");
    scanf("%d", &t0);
    if (t0 <= 0)
    {
        printf("Time quantum must be greater than 0.\n");
        exit(EXIT_FAILURE);
    }

    printf("Please enter a positive integer for the time quantum t1 for the Level 1 queue: ");
    scanf("%d", &t1);
    if (t1 <= 0)
    {
        printf("Time quantum must be greater than 0.\n");
        exit(EXIT_FAILURE);
    }

    printf("Please enter a positive integer for the time quantum t2 for the Level 2 queue: ");
    scanf("%d", &t2);
    if (t2 <= 0)
    {
        printf("Time quantum must be greater than 0.\n");
        exit(EXIT_FAILURE);
    }

    printf("Please enter a positive integer for the System time quantum W: ");
    scanf("%d", &W);
    if (W <= 0)
    {
        printf("Time quantum must be greater than 0.\n");
        exit(EXIT_FAILURE);
    }


//  2. Whenever there is a running process or the FCFS queue is not empty:

    while (current_process || job_queue || queue_0 || queue_1 || queue_2)
    {   
        //add the processes that have are ready to be processed onto the level x queue
        while (job_queue && job_queue->arrival_time <= timer) {
            process = deqPcb(&job_queue);          // dequeue process
            process->status = PCB_READY;

            // Check process priority and enqueue it to the corresponding queue
            switch (process->queue_priority) {
                case 0:
                    process-> remaining_quantum_time = t0;
                    queue_0 = enqPcb(queue_0, process);
                    break;
                case 1:
                    process-> remaining_quantum_time = t1;
                    process-> queue_arrival_time = timer;
                    queue_1 = enqPcb(queue_1, process);
                    break;
                case 2:
                    process-> remaining_quantum_time = t2;
                    process-> queue_arrival_time = timer;
                    queue_2 = enqPcb(queue_2, process);
                    break;
                default:
                    fprintf(stderr, "ERROR: Invalid process priority %d\n", process->queue_priority);
                    break;
            }
        }
        
//      i. If there is a currently running process;
        if (current_process)
        {
            if (current_process->queue_priority == 0){

                current_process->remaining_cpu_time--;
                current_process->remaining_quantum_time--;

                if (current_process->remaining_cpu_time <= 0){
                    terminatePcb(current_process);
                    printf("Process: %d TERMINATED\n", (int) current_process -> pid);
                    free(current_process);
                    current_process = NULL;
                } else if (current_process->remaining_quantum_time<=0){
                    suspendPcb(current_process);
                    current_process-> remaining_quantum_time = t1;
                    current_process-> queue_arrival_time = timer;
                    queue_1 = enqPcb(queue_1, current_process);
                    printf("Process: %d SUSPENDED\n", (int) current_process -> pid);
                    current_process = NULL;
                }
            } else if (current_process->queue_priority == 1){

                current_process->remaining_cpu_time--;
                current_process->remaining_quantum_time--;

                if (current_process->remaining_cpu_time <= 0){
                    terminatePcb(current_process);
                    printf("Process: %d TERMINATED\n", (int) current_process -> pid);
                    free(current_process);
                    current_process = NULL;
                } else if (current_process->remaining_quantum_time<=0){
                    suspendPcb(current_process);
                    current_process-> remaining_quantum_time = t2;
                    current_process-> queue_arrival_time = timer;
                    queue_2 = enqPcb(queue_2, current_process);
                    printf("Process: %d SUSPENDED\n", (int) current_process -> pid);
                    current_process = NULL;
                }
            } else if (current_process->queue_priority == 2){

                current_process->remaining_cpu_time--;
                current_process->remaining_quantum_time--;

                if (current_process->remaining_cpu_time <= 0){
                    terminatePcb(current_process);
                    printf("Process: %d TERMINATED\n", (int) current_process -> pid);
                    free(current_process);
                    current_process = NULL;
                } else if (current_process->remaining_quantum_time<=0){
                    suspendPcb(current_process);
                    current_process-> remaining_quantum_time = t2;
                    current_process-> queue_arrival_time = timer;
                    queue_2 = enqPcb(queue_2, current_process);
                    printf("Process: %d SUSPENDED\n", (int) current_process -> pid);
                    current_process = NULL;
                }
            }
            
        }

                /*** Checking the different queue levels, and set the current process correctly ***/

        //check if there exists a process in the level 0 queue
        if(queue_0){
            //if there exists no process currently, then take the process at the top
            if(!current_process){
                current_process = deqPcb(&queue_0);
                startPcb(current_process);
            }

            //if the existing process is a level 1 or 2 process, then pre empt
            else if (current_process->queue_priority == 1 || current_process->queue_priority == 2) {
                printf("PRE EMPTING THE PROCESS: %d\n", (int)current_process->pid);
                suspendPcb(current_process);
                if (current_process->queue_priority == 1) {
                    queue_1 = addFrontPcb(&queue_1, current_process);
                } else {
                    queue_2 = addFrontPcb(&queue_2, current_process);
                }
                current_process = deqPcb(&queue_0);
                startPcb(current_process);
            }
        }

        //check if there exists a process in the level 1 queue
        else if(queue_1){
            if(!current_process){
                current_process = deqPcb(&queue_1);
                startPcb(current_process);
            }

            //pre empt the level 2 process
            else if(current_process->queue_priority == 2){
                printf("PRE EMPTING THE PROCESS: %d\n", (int)current_process->pid);
                suspendPcb(current_process);
                queue_2 = addFrontPcb(&queue_2, current_process);
                current_process = deqPcb(&queue_1);
                startPcb(current_process);
            }

        }

        //check if there exists a process in the level 2 queue
        else if(queue_2){
            if(!current_process){
                current_process = deqPcb(&queue_2);
                startPcb(current_process);
            }
        }

        /**
         * This section onwards would be the Accumulated waiting time
         * NOTE: You might have to move this under the timer increment, TEST IT
         * NOTE: SINCE WE DONT HAVe TO STOP THE PROCESS WE JUST HAVE TO EnQUEUe it, this should be right
         * 
         * when accumulated waiting time>= W then we deque the queue one by one and upgrading it respectively
         */
        if (queue_1){
            int accumulated_waiting_time = timer - (int)queue_1->queue_arrival_time;
            if (accumulated_waiting_time >= W){
                printf("queue 1 front waiting time %d\n", accumulated_waiting_time);
                while (queue_1){
                    temp_process = deqPcb(&queue_1);
                    temp_process -> queue_arrival_time = timer;
                    queue_0 = enqPcb(queue_0, temp_process);
                    temp_process = NULL;
                }
                //OPTIONAL: queue_1 = NULL;
                while (queue_2){
                    temp_process = deqPcb(&queue_2);
                    temp_process -> queue_arrival_time = timer;
                    queue_0 = enqPcb(queue_0, temp_process);
                    temp_process = NULL;
                }
                //OPTIONAL: queue_2 = NULL;
            }
        }
        if (queue_2){
            int accumulated_waiting_time = timer - (int)queue_2->queue_arrival_time;
            if (accumulated_waiting_time >= W){
                printf("queue 2 front waiting time %d\n", accumulated_waiting_time);
                while (queue_2){
                    temp_process = deqPcb(&queue_2);
                    temp_process -> queue_arrival_time = timer;
                    queue_0 = enqPcb(queue_0, temp_process);
                    temp_process = NULL;
                }
            }
            //OPTIONAL: queue_2 = NULL;
        }


        
//      iii. Let the dispatcher sleep for one second;
        sleep(1);
        
//      iv. Increment the dispatcher's timer;
        timer++;
        

    }

//  print out average turnaround time and average wait time
    av_turnaround_time = av_turnaround_time / n;
    av_wait_time = av_wait_time / n;
    av_response_time = av_response_time / n;
    printf("average turnaround time = %f\n", av_turnaround_time);
    printf("average wait time = %f\n", av_wait_time);
    printf("average response time = %f\n", av_response_time);
    
//  3. Terminate the FCFS dispatcher
    exit(EXIT_SUCCESS);
}

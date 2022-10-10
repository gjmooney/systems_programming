/**
* Implements shortest job first and shortest job first live algorithms
*
* Completion time: 7 Hours
*
* @author Greg Mooney
* @version 1.0
*
*/

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Data Structures
typedef struct process_info {
    int* tick_info;
    int process_num;
    int tau;
    double alpha;
} process_info;

// Forward Declarations
void read_file(char* input_file);
void shortest_job_first();
void shortest_job_first_live();
void free_memory();

// Global Variable
int num_ticks;
int num_processes;
process_info* process_list;

// Implementation
void read_file(char* input_file) {
    FILE* file = fopen(input_file, "r");
    fscanf(file, "%d", &num_ticks);
    fscanf(file, "%d", &num_processes);

    process_list = malloc(sizeof(process_info) * num_processes);

    for (int i = 0; i < num_processes; ++i) {
        process_list[i].tick_info = malloc(sizeof(int) * num_ticks);
        fscanf(file, "%d", &process_list[i].process_num);
        fscanf(file, "%d", &process_list[i].tau);
        fscanf(file, "%lf", &process_list[i].alpha);

        for (int j = 0; j < num_ticks; ++j) {
            fscanf(file, "%d", &process_list[i].tick_info[j]);
        }
    }

    fclose(file);
}

void shortest_job_first() {
    process_info* order[num_processes];
    process_info * tmp;
    int time = 0;
    int min;
    int turnaround_time = 0;
    int waiting_time = 0;

    for (int i = 0; i < num_processes; ++i) {
        order[i] = &process_list[i];
    }

    printf("==Shortest-Job-First==\n");
    for (int i = 0; i < num_ticks; ++i) {
        for (int j = 0; j < num_processes - 1; ++j) {
            min = j;
            for (int k = j + 1; k < num_processes; ++k) {
                if (order[k]->tick_info[i] < order[min]->tick_info[i]) {
                    min = k;
                }
            }
            tmp = order[min];
            order[min] = order[j];
            order[j] = tmp;
        }

        printf("Simulating %dth tick of processes @ time %d\n", i, time);
        for (int j = 0; j < num_processes; ++j) {
            printf("Process %d took %d\n", order[j]->process_num, order[j]->tick_info[i]);
            time += order[j]->tick_info[i];

        }
        printf("\n");

        for (int j = 0; j < num_processes - 1; ++j) {
            waiting_time += order[j]->tick_info[i];
        }
    }
    for (int j = 0; j < num_processes; ++j) {
        turnaround_time = waiting_time + time;
    }

    printf("Turnaround time: %d\n", turnaround_time);
    printf("Waiting time: %d\n\n", waiting_time);
}

/**
 * tau_(n+1) = alpha * time_n + (1 - alpha) * tau_n
 */
void shortest_job_first_live() {
    process_info* order[num_processes];
    process_info * tmp;
    int time = 0;
    int min;
    int turnaround_time = 0;
    int waiting_time = 0;
    int est_error = 0;
    int diff;

    for (int i = 0; i < num_processes; ++i) {
        order[i] = &process_list[i];
    }

    printf("==Shortest-Job-First Live==\n");
    for (int i = 0; i < num_ticks; ++i) {
        for (int j = 0; j < num_processes - 1; ++j) {
            min = j;
            for (int k = j + 1; k < num_processes; ++k) {
                if (order[k]->tau < order[min]->tau) {
                    min = k;
                }
            }
            tmp = order[min];
            order[min] = order[j];
            order[j] = tmp;
        }

        printf("Simulating %dth tick of processes @ time %d\n", i, time);
        for (int j = 0; j < num_processes; ++j) {
            printf("Process %d was estimated for %d and took %d\n", order[j]->process_num, order[j]->tau, order[j]->tick_info[i]);
            time += order[j]->tick_info[i];
        }
        printf("\n");

        // Calculate estimation error and next tau
        for (int j = 0; j < num_processes; ++j) {
            diff = (order[j]->tau - order[j]->tick_info[i]);
            if (diff < 0) {diff *= -1;}
            est_error += diff;
            order[j]->tau = order[j]->alpha * order[j]->tick_info[i] + (1 - order[j]->alpha) * order[j]->tau;
        }

        for (int j = 0; j < num_processes - 1; ++j) {
            waiting_time += order[j]->tick_info[i];
        }
    }
    for (int j = 0; j < num_processes; ++j) {
        turnaround_time = waiting_time + time;
    }

    printf("Turnaround time: %d\n", turnaround_time);
    printf("Waiting time: %d\n", waiting_time);
    printf("Estimation Error: %d\n", est_error);


}

void free_memory() {
    for (int i = 0; i < num_processes; ++i) {
        free(process_list[i].tick_info);
    }

    free(process_list);
    process_list = NULL;
}

int main(int argc, char* argv[]) {
    int opt;
    char* input_file;

    while ((opt = getopt(argc, argv, ":")) != -1) {
        switch (opt) {
            case ':':
                printf("Option needs a value\n");
                break;
            case '?':
                printf("Unknown option: %c\n", optopt);
        }
    }

    // Read input file name
    input_file= argv[optind];
    optind++;

    // Extra arguments
    for(; optind < argc; optind++) {
        printf("Extra arguments: %s\n", argv[optind]);
    }

    int dflen;
    if(input_file != NULL){
        dflen = strlen(input_file);
        if(dflen >= 5
           && (strcmp(&input_file[dflen-4], ".txt") == 0)
           && (access(input_file, F_OK) != -1)){
            printf("Importing data from %s\n\n", input_file);
            read_file(input_file);
            shortest_job_first();
            shortest_job_first_live();
            free_memory();
        } else {
            printf("Data file has an invalid name or does not exist.\n");
            exit(1);
        }
    } else {
        printf("No data file name provided. This is a required field.\n");
        exit(1);
    }

    return 0;
}

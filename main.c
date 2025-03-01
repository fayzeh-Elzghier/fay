// Fayzeh Elzghier 1221160
// Mohammad Yaseen 1220911
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_PROCESSES 100
#define MAX_BURSTS 50
#define MAX_RESOURCES 50
#define TIME_QUANTUM 5
#define Max_rounds 40

// Structure to store process details
 struct  Process {
    int pid;
    int arrival_time;
    int priority;
    int cpu_bursts[MAX_BURSTS];
    int io_bursts[MAX_BURSTS];
    int cpu_burst_count;
    int io_burst_count;
    int waiting_time;
    int resource_count;
    int remaining_time;
};


typedef struct {
    int time;
    int pro;  // Process ID
    int statuse;  // 1 -> Requested, 0 -> Released
} Resource;

Resource R[MAX_RESOURCES];

struct Process processes[MAX_PROCESSES];
int process_count = 0;

// Function to parse bursts and resources
// Function to parse bursts and resources

void parse_bursts(char *burst_block,struct Process *current_process) {
    char *ptr = burst_block;

    while (*ptr) {
        // Skip whitespace
        while (isspace((unsigned char)*ptr)) {
            ptr++;
        }

        // Check for end of string
        if (*ptr == '\0') {
            break;
        }

        // Parse CPU bursts with embedded resources
        if (strncmp(ptr, "CPU{", 4) == 0) {
            ptr += 4; // Move past "CPU{"
            printf("In the CPU block\n");

            // Process the content inside the CPU block until the closing '}'
            while (*ptr != '}') {
                // Handle R or F followed by an index
                if (*ptr == 'R' || *ptr == 'F') {
                    char resource_type = *ptr; // 'R' or 'F'
                    ptr++; // Skip the character

                    if (*ptr == '[') {
                        ptr++; // Skip '['
                        if (isdigit((unsigned char)*ptr)) {
                            int index = atoi(ptr); // Extract the index
                            if (current_process->cpu_burst_count < MAX_BURSTS) {
                                // Store the negative index to indicate R/F
                                current_process->cpu_bursts[current_process->cpu_burst_count++] = -(index);
                            }
                            while (isdigit((unsigned char)*ptr)) {
                                ptr++; // Skip the digits in the index
                            }
                            if (*ptr == ']') {
                                ptr++; // Skip ']'
                            }
                        }
                    }
                } else if (isdigit((unsigned char)*ptr)) {
                    // Handle CPU burst time
                    int burst_time = atoi(ptr);
                    if (current_process->cpu_burst_count < MAX_BURSTS) {
                        current_process->cpu_bursts[current_process->cpu_burst_count++] = burst_time;
                        current_process->remaining_time += burst_time;
                    }
                    // Skip over the digits
                    while (isdigit((unsigned char)*ptr)) {
                        ptr++;
                    }
                }

                // Skip any commas or spaces
                while (*ptr == ',' || isspace((unsigned char)*ptr)) {
                    ptr++;
                }
            }

            // Move past the closing '}'
            if (*ptr == '}') {
                ptr++;
            }
        }
        // Parse IO bursts
        else if (strncmp(ptr, "IO{", 3) == 0) {
            ptr += 3; // Move past "IO{"
            while (isspace((unsigned char)*ptr)) {
                ptr++;
            }
            if (isdigit((unsigned char)*ptr)) {
                int burst_time = atoi(ptr);
                if (current_process->io_burst_count < MAX_BURSTS) {
                    current_process->io_bursts[current_process->io_burst_count++] = burst_time;
                }
                // Skip the digits
                while (isdigit((unsigned char)*ptr)) {
                    ptr++;
                }
            }

            // Move past the closing '}'
            if (*ptr == '}') {
                ptr++;
            }
        }

        // Move to the next token (skip spaces and commas)
        ptr += strcspn(ptr, " ,");
    }
}

// Function to read input file and parse processes
void read_input() {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("Processing line: %s", line);  // Debugging message
        struct Process *current_process = &processes[process_count];
        char *token = strtok(line, " ");
        if (token) current_process->pid = atoi(token);

        token = strtok(NULL, " ");
        if (token) current_process->arrival_time = atoi(token);

        token = strtok(NULL, " ");
        if (token) current_process->priority = atoi(token);

        token = strtok(NULL, "\n");
        if (token) {
            parse_bursts(token, current_process);  // Parse CPU bursts and resources
        }

        process_count++;
        if (process_count >= MAX_PROCESSES) {
            fprintf(stderr, "Error: Maximum number of processes exceeded.\n");
            break;
        }
    }

    fclose(file);
}


//////////queue function
////////////////////////////////
/////////////////////////////////
////////////////////////////////////////////////
// Queue-related definitions and function declarations
struct node {
    struct Process data;
    struct node* next;
};

struct queue {
    struct node* front;
    struct node* rear;
};

// Queue-related functions
struct node* newNode(struct Process data);
struct queue* createQueue();
int isEmpty(struct queue* q);
void enqueue(struct Process p, struct queue* q);
struct Process dequeue(struct queue* q);
void displayQueue(struct queue* q);
void freeQueue(struct queue* q);
void sortQueueByArrivalTime(struct queue* q);
void sortQueueByPriority(struct queue* q);

// Additional helper functions
void shiftLeft(int arr[], int size);
void simulate_cpu_scheduling(struct queue* q);

// Queue-related functions (same as before)
struct node* newNode(struct Process data) {
    struct node* temp = (struct node*)malloc(sizeof(struct node));
    temp->data = data;
    temp->next = NULL;
    return temp;
}

struct queue* createQueue() {
    struct queue* q = (struct queue*)malloc(sizeof(struct queue));
    q->front = q->rear = NULL;
    return q;
}

int isEmpty(struct queue* q) {
    return q->front == NULL;
}

void enqueue(struct Process p, struct queue* q) {
    struct node* temp = newNode(p);
    if (isEmpty(q)) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

struct Process dequeue(struct queue* q) {
    if (isEmpty(q)) {
        printf("Queue underflow\n");
        struct Process emptyProcess = {0}; // Return an empty process
        return emptyProcess;
    }
    struct node* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;

    struct Process data = temp->data;
    free(temp);
    return data;
}

void displayQueue(struct queue* q) {
    printf("\n");
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }

    struct node* temp = q->front;
    while (temp != NULL) {
        printf("PID: %d, Arrival: %d, Priority: %d, CPU Burst: %d,waiting time:%d ",
               temp->data.pid, temp->data.arrival_time, temp->data.priority, temp->data.cpu_bursts[0],temp->data.waiting_time);
        printf("\n");
        temp = temp->next;
    }
    printf("\n");
}

void freeQueue(struct queue* q) {
    struct node* temp = q->front;
    while (temp != NULL) {
        struct node* next = temp->next;
        free(temp);
        temp = next;
    }
    free(q);
}

void sortQueueByPriority(struct queue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty, nothing to sort.\n");
        return;
    }

    struct node* i;
    struct node* j;
    struct Process temp;

    // Bubble Sort on the linked list (stable version)
    for (i = q->front; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            // Change condition for ascending order, keeping stability
            if (i->data.priority > j->data.priority) {
                // Swap data
                temp = i->data;
                i->data = j->data;
                j->data = temp;
            }
        }
    }
}



void sortQueueByArrivalTime(struct queue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty, nothing to sort.\n");
        return;
    }

    struct node* i;
    struct node* j;
    struct Process temp;

    // Bubble Sort on the linked list (stable version)
    for (i = q->front; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            // Compare arrival times, ensuring stability
            if (i->data.arrival_time > j->data.arrival_time) {
                // Swap process data
                temp = i->data;
                i->data = j->data;
                j->data = temp;
            }
        }
    }
}

void shiftLeft(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
    arr[size - 1] = 0;
}

// Simulate CPU scheduling
int rounds=0;
void simulate_cpu_scheduling(struct queue* q1){
    if(isEmpty(q1)){
        printf("queue is empty!!!\n");
        return;
    }
 int timer=0;
 int counter1=0;
 int counter2=0;

struct queue* readyq=createQueue();
struct queue* waitq=createQueue();
struct queue* resultq=createQueue();
struct queue* rrq=createQueue();


sortQueueByArrivalTime(q1);

struct node* p=q1->front;



p=q1->front;


while(!isEmpty(q1)){

        enqueue(p->data,waitq);
        counter1++;
        dequeue(q1);
        p=q1->front;

}
sortQueueByArrivalTime(waitq);
int round=0;


while(!isEmpty(readyq) || !isEmpty(waitq)){

    if(isEmpty(readyq)){


        struct node* pr=waitq->front;
        enqueue(pr->data,readyq);

                dequeue(waitq);
                pr=waitq->front;

        while(pr!=NULL){
            if(pr->data.arrival_time<=timer&&counter1!=0){

                enqueue(pr->data,readyq);

                dequeue(waitq);
                pr=pr->next;

            }

       else{
        break;
       }

        }


    }


  else if(!isEmpty(readyq)){
round++;

       // displayQueue(waitq);
         struct node* pr=waitq->front;
        while(pr!=NULL){

            if(pr->data.arrival_time<=timer){

                enqueue(pr->data,readyq);

                dequeue(waitq);
                pr=pr->next;
            }
            else{
                break;
            }
        }

    sortQueueByPriority(readyq);


    //Round Robin

    int minp=1000;
    int countp=0;
    struct node* pp=readyq->front;

    while(pp!=NULL){
        if(pp->data.priority<minp)minp=pp->data.priority;
        pp=pp->next;
    }

    pp=readyq->front;

    while(pp!=NULL){
        if(pp->data.priority==minp)countp++;
        pp=pp->next;
    }
    pp=readyq->front;

    ////////////////////////
    ////////////////////////////////////
    ///////////////////////////////////////////
    ///////////////////////////////////////////////
  if(countp>1){

    for(int i=0;i<countp;i++){

            pp=readyq->front;


             enqueue(pp->data,rrq);
                dequeue(readyq);


    }


while(!isEmpty(rrq)){
int rtime=0;

if(rrq->front->data.cpu_bursts[0]<=TIME_QUANTUM){





    struct node* prr =rrq->front;

if(prr->data.cpu_bursts[0]==0){
    dequeue(rrq);
    continue;
}
///////////////////////////if less than zero
///////////////////////////////////////////
///////////////////////////if less than zero
///////////////////////////////////////////
///////////////////////////if less than zero
///////////////////////////////////////////
///////////////////////////if less than zero
///////////////////////////////////////////


             else if(prr->data.cpu_bursts[0]<0){

      while (prr != NULL && prr->data.cpu_bursts[0] < 0) { // Check prr != NULL
    if (R[prr->data.cpu_bursts[0] * -1].statuse == 1) {


        if (R[prr->data.cpu_bursts[0] * -1].pro == prr->data.pid) {
            R[prr->data.cpu_bursts[0] * -1].pro = 0;
            R[prr->data.cpu_bursts[0] * -1].statuse = 0;

            shiftLeft(prr->data.cpu_bursts, 20);
            continue;
        }




    else if (R[prr->data.cpu_bursts[0] * -1].pro != prr->data.pid) {



prr->data.arrival_time=timer;
            enqueue(prr->data, waitq);
        int pid=prr->data.pid;

            dequeue(rrq);


            prr=rrq->front;


            if (prr == NULL) {
     rounds=rounds+1;
if(rounds=1){
//recovery of dead lock
 int tt=timer;
 printf("VICTIM PROCESSE =%d\n",pid);
        printf("time of dead lock=%d\n",tt);

printf("there is an dead lock\n");

for(int k=0;k<MAX_RESOURCES;k++){

   if(R[k].pro==pid){
     R[k].pro = 0;
            R[k].statuse = 0;
   }

}

}

            }
        }
    } else if (R[prr->data.cpu_bursts[0] * -1].statuse == 0) {
        R[prr->data.cpu_bursts[0] * -1].pro = prr->data.pid;
        R[prr->data.cpu_bursts[0] * -1].statuse = 1;
        R[prr->data.cpu_bursts[0] * -1].time=rtime;
        shiftLeft(prr->data.cpu_bursts, 20);
    }
}
if(prr!=NULL){

                if(prr!=NULL&&prr->data.cpu_bursts[0]>TIME_QUANTUM){


                         struct Process prr=rrq->front->data;
         struct Process prr2=rrq->front->data;
                prr.cpu_bursts[0]=TIME_QUANTUM;


               if(prr.arrival_time>=timer){
   int avc1=prr.arrival_time;
         prr.waiting_time=prr.arrival_time-avc1;
          int t2=prr.arrival_time-timer;
             timer=t2+timer+prr.cpu_bursts[0];

       }
        else{

           int avc1=prr.arrival_time;
         prr.arrival_time=timer;
         prr.waiting_time=prr.arrival_time-avc1;
          timer=timer+prr.cpu_bursts[0];
         }
          enqueue(prr,resultq);
         /////////////////////////////////////////
           prr2.cpu_bursts[0]=prr2.cpu_bursts[0]-TIME_QUANTUM;

             enqueue(prr2,rrq);
                dequeue(rrq);
                    continue;


                }}



             }
///////////////////////////if less than zero
///////////////////////////////////////////
///////////////////////////if less than zero
///////////////////////////////////////////
///////////////////////////if less than zero
///////////////////////////////////////////
///////////////////////////if less than zero
///////////////////////////////////////////
if(prr!=NULL){
if(prr->data.cpu_bursts[0]==0){
        dequeue(rrq);
   continue;
}






               if(prr->data.arrival_time>=timer){
 int avc1=prr->data.arrival_time;

         prr->data.waiting_time=prr->data.arrival_time-avc1;
    int t2=prr->data.arrival_time-timer;
    timer=t2+timer+prr->data.cpu_bursts[0];

}
    else if(prr->data.arrival_time<timer&&prr!=NULL){

    int avc1=prr->data.arrival_time;
         prr->data.arrival_time=timer;
         prr->data.waiting_time=prr->data.arrival_time-avc1;
         timer=timer+prr->data.cpu_bursts[0];
     }
     if(prr!=NULL){

        enqueue(prr->data,resultq);

    int cp2=prr->data.cpu_bursts[0];
    shiftLeft(prr->data.cpu_bursts,20);


      if(prr->data.cpu_bursts[0]!=0){
          if(prr->data.cpu_bursts[0]<0){


                prr->data.arrival_time=timer;
                enqueue(prr->data,waitq);

          }
          else if(prr->data.cpu_bursts[0]>0){
               prr->data.arrival_time=timer+prr->data.io_bursts[0];
             enqueue(prr->data,waitq);
         shiftLeft(prr->data.io_bursts,20);
          sortQueueByArrivalTime(waitq);

          }

              }



     }

      dequeue(rrq);

}


}
else if(rrq->front->data.cpu_bursts[0]>TIME_QUANTUM&&rrq->front!=NULL){

         struct Process prr=rrq->front->data;
         struct Process prr2=rrq->front->data;
                prr.cpu_bursts[0]=TIME_QUANTUM;


               if(prr.arrival_time>=timer){
   int avc1=prr.arrival_time;

         prr.waiting_time=prr.arrival_time-avc1;
          int t2=prr.arrival_time-timer;
             timer=t2+timer+prr.cpu_bursts[0];

       }
        else{

           int avc1=prr.arrival_time;
         prr.arrival_time=timer;
         prr.waiting_time=prr.arrival_time-avc1;
          timer=timer+prr.cpu_bursts[0];
         }
          enqueue(prr,resultq);
         /////////////////////////////////////////
           prr2.cpu_bursts[0]=prr2.cpu_bursts[0]-TIME_QUANTUM;

             enqueue(prr2,rrq);
                dequeue(rrq);

}





}

  }

    ///////////////////////////////////
    if(!isEmpty(readyq)){


      struct Process p2=readyq->front->data;

if(p2.cpu_bursts[0]==0){
    dequeue(readyq);
    continue;
}

while(p2.cpu_bursts[0]!=0){


     if(p2.cpu_bursts[0]<0){

while(p2.cpu_bursts[0]<0){
        if(R[p2.cpu_bursts[0]*-1].statuse==0){
                 R[p2.cpu_bursts[0]*-1].pro=p2.pid;
  R[p2.cpu_bursts[0]*-1].statuse=1;
shiftLeft(p2.cpu_bursts,20);
        }
        else if(R[p2.cpu_bursts[0]*-1].statuse==1){
            if(R[p2.cpu_bursts[0]*-1].pro==p2.pid){
                    R[p2.cpu_bursts[0]*-1].statuse=0;
                     R[p2.cpu_bursts[0]*-1].pro=0;
                shiftLeft(p2.cpu_bursts,20);

            }
            else if(R[p2.cpu_bursts[0]*-1].pro!=p2.pid){
                printf("!!!!!!!!!!!!!!!!!!!!!!!\n");
            }

        }


}
     }
     if(p2.cpu_bursts[0]==0)continue;

     if(p2.arrival_time>=timer){
 int avc1=p2.arrival_time;

         p2.waiting_time=p2.arrival_time-avc1;
    int t2=p2.arrival_time-timer;
    timer=t2+timer+p2.cpu_bursts[0];

}

else{

int avc1=p2.arrival_time;
         p2.arrival_time=timer;
         p2.waiting_time=p2.arrival_time-avc1;
         timer=timer+p2.cpu_bursts[0];
}

enqueue(p2,resultq);





shiftLeft(p2.cpu_bursts,20);


if(p2.cpu_bursts[0]>0){
int cp2=p2.cpu_bursts[0];
    p2.arrival_time=timer+p2.io_bursts[0];
p2.cpu_bursts[0]=cp2;
    enqueue(p2,waitq);
    shiftLeft(p2.io_bursts,20);
sortQueueByArrivalTime(waitq);
shiftLeft(p2.cpu_bursts,20);
}



}


dequeue(readyq);








    }





  }



}

struct node* avg=resultq->front;

double sumOfWaitTime=0;
double avgTime=0;
while(avg!=NULL){

    sumOfWaitTime=sumOfWaitTime+avg->data.waiting_time;
    avg=avg->next;

}

avgTime=sumOfWaitTime/process_count;




printf("average waiting time=%f\n",avgTime);
printf("gantt chart:\n");
avg=resultq->front;
while(avg!=NULL){

    printf("  |   P%d   |  ",avg->data.pid);

    avg=avg->next;

}
avg=resultq->front;
 printf("\n");
while(avg!=NULL){

    printf("  %d        %d",avg->data.arrival_time,avg->data.arrival_time+avg->data.cpu_bursts[0]);

    avg=avg->next;

}

 printf("\n");

printf("%d\n",timer);
displayQueue(resultq);
}

void storeInQ(struct queue* q){
  for (int i = 0; i < process_count; i++) {

        enqueue(processes[i],q);

    }




}

void print_processes() {
    printf("\nProcesses Information:\n");
    for (int i = 0; i < process_count; i++) {
        struct Process *p = &processes[i];
        printf("Process ID: %d\n", p->pid);
        printf("Arrival Time: %d\n", p->arrival_time);
        printf("Priority: %d\n", p->priority);

        printf("CPU Bursts: ");
        for (int j = 0; j < p->cpu_burst_count; j++) {
            printf("%d ", p->cpu_bursts[j]);
        }
        printf("\n");

        printf("IO Bursts: ");
        for (int j = 0; j < p->io_burst_count; j++) {
            printf("%d ", p->io_bursts[j]);
        }
        printf("\n");

        printf("\n\n");
    }
}
void fillResourceArray() {
    // Initialize resources
    for (int i = 0; i < MAX_RESOURCES; i++) {
        R[i].pro = -1;
        R[i].statuse = 0;
        R[i].time = -1;
    }

    // Process each process
    for (int i = 0; i < process_count; i++) {
        struct Process *p = &processes[i];
        int j = 0;

        // Iterate through the CPU bursts
        while (j < p->cpu_burst_count) {
            if (p->cpu_bursts[j] < 0) {  // If we encounter a negative number, it's a resource request
                int resource_index = -(p->cpu_bursts[j]);  // Get the resource index (negative to positive)

                // Ensure that the resource index is valid
                if (resource_index < MAX_RESOURCES) {
                    // If the resource is already assigned to another process, the current process must wait
                    if (R[resource_index].pro != -1) {
                        printf("Process %d requests Resource %d, but it's already assigned to Process %d. Process %d will have to wait.\n",p->pid, resource_index, R[resource_index].pro, p->pid);
                        // Move to the next burst without processing this one
                        j++;
                        continue;  // Skip to the next burst in the CPU burst array
                    } else {
                        // Resource is available, assign it to the current process
                        R[resource_index].pro = p->pid;  // Assign the resource to the process
                        R[resource_index].statuse = 1;   // Resource is requested
                        j++;  // Move to the next CPU burst to fetch the burst time

                        if (j < p->cpu_burst_count && p->cpu_bursts[j] > 0 ) {
                            // Store the burst time associated with the resource
                            R[resource_index].time = p->cpu_bursts[j];  // Store the burst time
                            printf("Resource %d is requested for %d time units by Process %d\n", resource_index, R[resource_index].time, p->pid);
                        }

                      else  if (j < p->cpu_burst_count && p->cpu_bursts[j++] < 0 ) {
                            // Store the burst time associated with the resource
                            R[resource_index].time = p->cpu_bursts[j++];  // Store the burst time
                            printf("Resource %d is requested for %d time units by Process %d\n", resource_index, R[resource_index].time, p->pid);
                        }
                    }
                }
            } else {
                // Regular CPU burst, move to the next burst
                j++;
            }
        }
    }

    // Print the resource information
    printf("\nResource Information:\n");
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (R[i].pro != -1) {  // Only print resources with valid processes
            printf("Resource Index: %d, Time: %d, Process: %d, Status: %d\n", i, R[i].time, R[i].pro, R[i].statuse);
        }
    }
}


int main() {
    struct queue* q = createQueue();




    read_input();
    print_processes();
    //fillResourceArray();

storeInQ(q);

 simulate_cpu_scheduling(q);
printf("====================\n");
printf("R[1] statues:%d\n",R[1].statuse);
printf("R[1] pro:%d\n",R[1].pro);
printf("R[1] time:%d\n",R[1].time);
printf("R[2] statues:%d\n",R[2].statuse);
printf("R[2] pro:%d\n",R[2].pro);
printf("R[2] time:%d\n",R[2].time);
    printf("==================\n");
    //print_processes();
    printf("=================\n");


    return 0;
}




// Function to print all processes and their details










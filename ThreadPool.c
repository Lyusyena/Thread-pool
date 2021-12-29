#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define THREAD_NUM 8

typedef struct Task {
    int* n;
    int n1;
} Task;

Task taskQueue[256];
int taskCount = 0;
int queueSize = 0;
pthread_mutex_t queueMutex;
pthread_cond_t queueCond;

int FibTask(int n){        
    usleep(10000);
    unsigned int i, a1, a2, b;
    a1 = 1;
    a2 = 1;
    printf("%d %d ", a1, a2); 
    if (n <= 1) { 
      return 1;
       }
    for (i = 3; i <= n; ++i) 
    {
        printf("%d ", a1 + a2);
        b = a1;
        a1 = a2;
        a2 = b + a1;
    }
    printf("\n");

 } 

 int FactorialTask(int n1){
   if (n1 <= 1) { 
    return 1;
}
   else {
    return n1 * FactorialTask(n1 - 1);
    }
    printf("Factorial = %u\n", FactorialTask(5));
}



void* executeTask(Task* task) {
    int result = task->a + task->b;
    printf("The sum of %d and %d is %d\n", task->a, task->b, result);
}

void submitTask(Task task) {
    pthread_mutex_lock(&queueMutex);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&queueMutex);
    pthread_cond_signal(&queueCond);
}

void startThread() {
    while(1) {
        Task task;
        pthread_mutex_lock(&queueMutex);
        while(queueSize == 0) {
            pthread_cond_wait(&queueCond, &queueMutex);
        }

        task = taskQueue[0];
        for(int i = 0; i < queueSize-1; ++i) {
            taskQueue[i] = taskQueue[i+1];
        }
        queueSize--;
        pthread_mutex_unlock(&queueMutex);

        executeTask(&task);
    }
}

int main(int argc, char** argv) {
    pthread_t threadPool[THREAD_NUM];
    pthread_mutex_init(&queueMutex, NULL);
    pthread_cond_init(&queueCond, NULL);
    for(int i = 0; i < THREAD_NUM; ++i) {
        if(pthread_create(&threadPool[i], NULL, &startThread, NULL)!= 0) {
            printf("Thread creation error\n");
        }
    }
    int n = 10;
    srand(time(NULL));

     for(int i = 0; i < 100; ++i ) {
        Task t1;
        t1.n = rand() % 100;
        submitTask(t1);
    }

     for(int j = 0; j < 10; j++){         
        Task t2;
        t2.n1 =  rand() % 100;
        submitTask(t2);
    }


    for(int i = 0; i < THREAD_NUM; ++i) {
        if(pthread_join(threadPool[i], NULL) != 0) {
            printf("Thread join error\n");
        }
    }
    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&queueCond);

    return 0;
}
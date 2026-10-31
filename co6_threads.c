#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "monitor.h"

int counter = 0;

pthread_mutex_t lock;

sem_t semaphore;

void *worker(void *arg) {

    for (int i = 0; i < 100000; i++) {

        pthread_mutex_lock(&lock);

        counter++;

        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

void *semaphore_worker(void *arg) {

    sem_wait(&semaphore);

    printf("Thread entered critical section.\n");

    sleep(1);

    printf("Thread leaving critical section.\n");

    sem_post(&semaphore);

    return NULL;
}

void thread_demo() {

    pthread_t t1, t2;

    printf("\n===== THREAD SYNCHRONIZATION =====\n");

    pthread_mutex_init(&lock, NULL);

    counter = 0;

    pthread_create(&t1,
                   NULL,
                   worker,
                   NULL);

    pthread_create(&t2,
                   NULL,
                   worker,
                   NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final Counter: %d\n",
           counter);

    pthread_mutex_destroy(&lock);

    printf("\n===== SEMAPHORE =====\n");

    sem_init(&semaphore,
             0,
             1);

    pthread_create(&t1,
                   NULL,
                   semaphore_worker,
                   NULL);

    pthread_create(&t2,
                   NULL,
                   semaphore_worker,
                   NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&semaphore);
}

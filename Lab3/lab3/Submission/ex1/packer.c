#include "packer.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

#define MAX_BALL_PER_BOX 2

// You can declare global variables here
sem_t mutex;
sem_t barrier1[4];
sem_t barrier2[4];
sem_t queueEmpty[4];
int count[4] = {0,0,0,0};
enum Color {RED = 1, GREEN = 2, BLUE = 3};
int queue[4][MAX_BALL_PER_BOX];

void synchronize_box(int color, int id, int* other_id);

void packer_init(void) {
    // Write initialization code here (called once at the start of the program).
    sem_init(&mutex, 0, 1);
    for (int i = 0; i <= 3;i ++){
        sem_init(&barrier1[i], 0, 0);
        sem_init(&barrier2[i], 0, 1);
        sem_init(&queueEmpty[i], 0, 2);
    }
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    sem_destroy(&mutex);
    for (int i = 0; i <= 3;i ++){
        sem_destroy(&barrier1[i]);
        sem_destroy(&barrier2[i]);
        sem_destroy(&queueEmpty[i]);
    }
}

int pack_ball(int colour, int id) {
    // Write your code here.
    int result;
    synchronize_box(colour, id, &result);
    return result;
}

void synchronize_box(int colour, int id, int *other_id){
    sem_wait(&queueEmpty[colour]);
    
    sem_wait(&mutex);
    queue[colour][count[colour]] = id;
    count[colour]++;
    if (count[colour] == 2) {
        sem_wait(&barrier2[colour]);
        sem_post(&barrier1[colour]);
    }
    sem_post(&mutex);
    
    sem_wait(&barrier1[colour]);
    sem_post(&barrier1[colour]);

    sem_wait(&mutex);
    if (queue[colour][0] == id)
        *other_id = queue[colour][1];
    else
        *other_id = queue[colour][0];
    sem_post(&mutex);

    sem_wait(&mutex);
    count[colour]--;
    if (count[colour] == 0) {
        sem_wait(&barrier1[colour]);
        sem_post(&barrier2[colour]);

        sem_post(&queueEmpty[colour]);
        sem_post(&queueEmpty[colour]);
    }
    sem_post(&mutex);

    sem_wait(&barrier2[colour]);
    sem_post(&barrier2[colour]);
}


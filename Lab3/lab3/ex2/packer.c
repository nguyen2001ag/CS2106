#include <semaphore.h>
#include <stddef.h>

#include "packer.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_QUEUE_SIZE 1000

// You can declare global variables here
sem_t mutex;
sem_t barrier1[4];
sem_t barrier2[4];
sem_t queueEmpty[4];
int count[4] = {0,0,0,0};
enum Color {RED = 1, GREEN = 2, BLUE = 3};
int queue[4][MAX_QUEUE_SIZE];
int nBallsPerPack = 0;

// void synchronize_box(int colour, int id, int *other_id);

void packer_init(int balls_per_pack) {
    sem_init(&mutex, 0, 1);
    for (int i = 0; i <= 3;i ++){
        sem_init(&barrier1[i], 0, 0);
        sem_init(&barrier2[i], 0, 1);
        sem_init(&queueEmpty[i], 0, balls_per_pack);
    }
    nBallsPerPack = balls_per_pack;
}

void packer_destroy(void) {
    sem_destroy(&mutex);
    for (int i = 0; i <= 3;i ++){
        sem_destroy(&barrier1[i]);
        sem_destroy(&barrier2[i]);
        sem_destroy(&queueEmpty[i]);
    }
}

void pack_ball(int colour, int id, int *other_ids) {
    sem_wait(&queueEmpty[colour]);
    
    sem_wait(&mutex);
    queue[colour][count[colour]] = id;
    count[colour]++;
    if (count[colour] == nBallsPerPack) {
        sem_wait(&barrier2[colour]);
        sem_post(&barrier1[colour]);
    }
    sem_post(&mutex);
    
    sem_wait(&barrier1[colour]);
    sem_post(&barrier1[colour]);

    sem_wait(&mutex);
    for (int i = 0, j = 0; i < nBallsPerPack; i++, j++){
        if (queue[colour][i] == id){
            j--;
            continue;
        }
        other_ids[j] = queue[colour][i];
    }
    sem_post(&mutex);

    sem_wait(&mutex);
    count[colour]--;
    if (count[colour] == 0) {
        sem_wait(&barrier1[colour]);
        sem_post(&barrier2[colour]);

        for (int i = 0; i < nBallsPerPack; i++){
            sem_post(&queueEmpty[colour]);
        }
    }
    sem_post(&mutex);

    sem_wait(&barrier2[colour]);
    sem_post(&barrier2[colour]);
}

// void synchronize_box(int colour, int id, int *other_ids){
    
// }

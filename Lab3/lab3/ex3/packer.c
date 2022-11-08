#include <semaphore.h>
#include <stddef.h>

#include "packer.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_QUEUE_SIZE 1000
#define MAX_BALL_PER_BOX 1

// You can declare global variables here

// Unfinished

sem_t mutex;
sem_t mutex1;
sem_t barrier1[5];
sem_t barrier2[5];
sem_t colourBall[5];
int count[5] = {0,0,0,0,0};
enum Color {RED = 1, GREEN = 2, BLUE = 3, BLACK = 4};
int queue[5][MAX_QUEUE_SIZE];
int queueIdx[2];
int ballInBox[2];
sem_t boxEmpty [2];


void synchronize_box(int colour, int id, int *other_id);
// You can declare global variables here

void packer_init(void) {
    // Write initialization code here (called once at the start of the program).
    sem_init(&mutex, 0, 1);
    sem_init(&mutex1, 0, 1);
    for (int i = 0; i <= 4;i ++){
        sem_init(&colourBall[i], 0, 1);
        sem_init(&barrier1[i], 0, 0);
        sem_init(&barrier2[i], 0, 1);
    }
    for (int i = 0; i < 2; i++){       
        sem_init(&boxEmpty[i], 0, 1);
        queueIdx[i] = 0;
        ballInBox[i] = 0;
    }
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    sem_destroy(&mutex);
    for (int i = 0; i <= 4;i ++){
        sem_destroy(&colourBall[i]);
        sem_destroy(&barrier1[i]);
        sem_destroy(&barrier2[i]);
    }
}

int pack_ball(int colour, int id) {
    // Write your code here.
    int result = -1;
    synchronize_box(colour, id, &result);
    return result;
}

void synchronize_box(int colour, int id, int *other_ids){
    sem_wait(&mutex);
    queue[colour][count[colour]] = id;
    count[colour]++;
    
    if ((colour == RED && count[GREEN] > queueIdx[0])
        || (colour == GREEN && count[RED] > queueIdx[0])
        || (colour == BLACK && count[BLUE] > queueIdx[1])
        || (colour == BLUE && count[BLACK] > queueIdx[1]) ) {
        sem_wait(&boxEmpty[colour/3]);
        sem_wait(&barrier2[colour]);
        sem_post(&barrier1[colour]);
        // sem_wait(&boxEmpty[colour/3]);
        // sem_post(&colourBall[colour]);
    }

    sem_post(&mutex);
    

    sem_wait(&barrier1[colour]);
    sem_post(&barrier1[colour]);

    sem_wait(&colourBall[colour]);
    sem_wait(&mutex1);
    if (colour == RED ){
        *other_ids = queue[GREEN][queueIdx[0]];
        ballInBox[0]++;
    }
    else if (colour == GREEN){
        *other_ids = queue[RED][queueIdx[0]];
        ballInBox[0]++;
    }
    else if (colour == BLUE){
        *other_ids = queue[BLACK][queueIdx[1]];
        ballInBox[1]++;
    }
    else if (colour == BLACK) {
        *other_ids = queue[BLUE][queueIdx[1]];
        ballInBox[1]++;
    }
    sem_post(&mutex1);
    sem_post(&colourBall[colour]);


    // printf("Number of ball in box: %d\n", ballInBox[0]);
    

    sem_wait(&mutex1);
    if (ballInBox[0] == 2 && (colour == RED || colour == GREEN)) {
        queueIdx[0]++;
        printf("I am at queueIDX: %d\n", queueIdx[0]);
        ballInBox[0] = 0;
        sem_wait(&barrier1[colour]);
        sem_post(&barrier2[colour]);
        sem_post(&colourBall[GREEN]);
        sem_post(&colourBall[RED]);
    }
    else if (ballInBox[1] == 2 && (colour == BLACK || colour == BLUE)){
        queueIdx[1]++;
        printf("I am at queueIDX: %d\n", queueIdx[1]);
        ballInBox[1] = 0;
        // if (colour == BLUE){
        //     sem_wait(&barrier1[BLACK]);
        //     sem_post(&barrier2[BLACK]);
        // }
        // else if (colour == BLACK){
        //     sem_wait(&barrier1[BLUE]);
        //     sem_post(&barrier2[BLUE]);
        // }
        sem_wait(&barrier1[colour/3]);
        sem_post(&barrier2[colour/3]);
        sem_post(&colourBall[BLUE]);
        sem_post(&colourBall[BLACK]);
    }
    sem_post(&mutex1);

    sem_wait(&barrier2[colour]);
    sem_post(&barrier2[colour]);
    // if (colour == RED){
    //     sem_post(&barrier2[GREEN]);
    // }
    // else if (colour == GREEN){
    //     sem_post(&barrier2[RED]);
    // }
    // else if (colour == BLUE){
    //     sem_post(&barrier2[BLACK]);
    // }
    // else if (colour == BLACK){
    //     sem_post(&barrier2[BLUE]);
    // }
}

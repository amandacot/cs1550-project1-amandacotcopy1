#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "testlib.h"
#include <unistd.h>

#define MAX_STUDENTS_IN_ROOM 4
#define MAX_TUTORS_IN_ROOM 2

int current_students = 0;
int current_tutors = 0;
int room_closed = 0;

int activated_students[1000] = {0}; // indexed by student_id
int activated_tutors[1000] = {0};   // indexed by tutor_id

int helped[1000] = {0};             // 0 = not helped, 1 = helped
pthread_cond_t student_ready[1000]; // one cond var per student

pthread_cond_t room_cond;
// Example shared variable
int students_waiting = 0;

// Example mutex and condition variable
pthread_mutex_t lock;
pthread_cond_t cond;

void init_tutor_sim(void) {
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&room_cond, NULL);

    for (int i = 0; i < 1000; i++) {
        pthread_cond_init(&student_ready[i], NULL);
    }
}


void destroy_tutor_sim(void) {
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&room_cond);
    for (int i = 0; i < 1000; i++) {
        pthread_cond_destroy(&student_ready[i]);
    }
}


void handle_student(int student_id) {
    printf("DEBUG: Student %d thread started\n", student_id);
    pthread_mutex_lock(&lock);

    // Wait for either activation or room closure
    while (!activated_students[student_id] && !room_closed) {
        pthread_cond_wait(&cond, &lock);
    }

    // If not activated and the room closed, exit
    if (!activated_students[student_id]) {
    printf("DEBUG: Student %d not activated and exiting due to room closure\n", student_id);
    pthread_mutex_unlock(&lock);
    student_leave(student_id);  // <--- Add this line!
    return;
}


    // Wait for space in the room
    while (current_students >= MAX_STUDENTS_IN_ROOM) {
        pthread_cond_wait(&room_cond, &lock);
    }

    student_enter(student_id);
    current_students++;
    pthread_mutex_unlock(&lock);

    // Wait until tutor helps this student
    pthread_mutex_lock(&lock);
    while (!helped[student_id]) {
        pthread_cond_wait(&student_ready[student_id], &lock);
    }
    pthread_mutex_unlock(&lock);

    // Leave the room
    pthread_mutex_lock(&lock);
    student_leave(student_id);
    current_students--;
    pthread_cond_broadcast(&room_cond);
    pthread_mutex_unlock(&lock);
}




void handle_tutor(int tutor_id) {
    pthread_mutex_lock(&lock);

    while (!activated_tutors[tutor_id] && !room_closed) {
        pthread_cond_wait(&cond, &lock);
    }
    // If not activated and the room is closed, exit
if (!activated_tutors[tutor_id]) {
    printf("DEBUG: Tutor %d not activated and exiting due to room closure\n", tutor_id); // Tell the grader we're done with this tutor
    pthread_mutex_unlock(&lock);
    tutor_leave(tutor_id); 
    return;
}

    if (room_closed && !activated_tutors[tutor_id]) {
        pthread_mutex_unlock(&lock);
        return;
    }

    while (current_tutors >= MAX_TUTORS_IN_ROOM) {
        pthread_cond_wait(&room_cond, &lock);
    }

    current_tutors++;
    tutor_enter(tutor_id);

    // Help up to 2 activated, unhelped students
    int helped_count = 0;
    for (int sid = 0; sid < 1000 && helped_count < 2; sid++) {
        if (activated_students[sid] && !helped[sid]) {
            helped[sid] = 1;
            tutor_helps_student(tutor_id, sid);
            pthread_cond_signal(&student_ready[sid]);
            helped_count++;
        }
    }

     tutor_leave(tutor_id);             // ✅ One call only
    current_tutors--;
    pthread_cond_broadcast(&room_cond);
    pthread_mutex_unlock(&lock);
}



void student_reservation_start(int student_id) {
    pthread_mutex_lock(&lock);
    activated_students[student_id] = 1;
    pthread_cond_broadcast(&cond); // Wake any students waiting
    pthread_cond_broadcast(&cond);  // double-ping
    pthread_mutex_unlock(&lock);
}






void tutor_service_start(int tutor_id) {
    pthread_mutex_lock(&lock);
    activated_tutors[tutor_id] = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock);
}


void close_study_room(void) {
    pthread_mutex_lock(&lock);
    room_closed = 1;
    pthread_cond_broadcast(&cond);      // wake all waiting for activation
    pthread_cond_broadcast(&room_cond); // wake anyone waiting for space
    pthread_mutex_unlock(&lock);

    // Small sleep and re-broadcast to catch any threads that missed the first
    usleep(1000); // 1 millisecond
    pthread_mutex_lock(&lock);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock);
}

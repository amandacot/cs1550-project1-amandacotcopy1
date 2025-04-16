#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "testlib.h"

#define MAX_STUDENTS_IN_ROOM 4
#define MAX_TUTORS_IN_ROOM 2
#define MAX_STUDENTS 1000

// Shared State
int current_students = 0;
int current_tutors = 0;
int room_closed = 0;

int activated_students[MAX_STUDENTS] = {0};
int activated_tutors[MAX_STUDENTS] = {0};
int entered[MAX_STUDENTS] = {0};
int helped[MAX_STUDENTS] = {0};

pthread_cond_t student_ready[MAX_STUDENTS];
pthread_cond_t room_cond;

// Example shared variable
int students_waiting = 0;

// Example mutex and condition variable
pthread_mutex_t lock;
pthread_cond_t cond;

void init_tutor_sim(void) {
    // Initialize synchronization primitives
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_cond_init(&room_cond, NULL);

    for (int i = 0; i < MAX_STUDENTS; i++) {
        pthread_cond_init(&student_ready[i], NULL);
    }
}

void destroy_tutor_sim(void) {
    // Destroy synchronization primitives
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    pthread_cond_destroy(&room_cond);
    for (int i = 0; i < MAX_STUDENTS; i++) {
        pthread_cond_destroy(&student_ready[i]);
    }
}

void handle_student(int student_id) {
    // TODO: Wait until either:
    //       - The student's reservation has started OR
    //       - The study room is closed
    //
    //       Use a condition variable inside a loop while holding the mutex.
    //       If the room is closed and the student was not activated, leave immediately.

    pthread_mutex_lock(&lock);
    while (!activated_students[student_id] && !room_closed) {
        pthread_cond_wait(&cond, &lock);
    }

    if (!activated_students[student_id]) {
        pthread_mutex_unlock(&lock);
        student_leave(student_id); 
        return;
    }

    
    // TODO: Wait until the room has space for students (max 4 at a time)
    //       - Use condition variable and a counter for current students in the room.
    
    while (current_students >= MAX_STUDENTS_IN_ROOM) {
        pthread_cond_wait(&room_cond, &lock);
    }

    // TODO: Enter the room
    //       - Call student_enter(student_id)
    //       - Increment counter for students in the room
    
    entered[student_id] = 1;
    student_enter(student_id);
    current_students++;

    pthread_mutex_unlock(&lock);
    
    // TODO: Wait until a tutor helps this student
    //       - Use a mechanism to match tutor and student
    //       - You may use a queue or per-student flag/signaling
    
    pthread_mutex_lock(&lock);
    while (!helped[student_id]) {
        pthread_cond_wait(&student_ready[student_id], &lock);
    }
    pthread_mutex_unlock(&lock);
    
    // TODO: Leave the room
    //       - Call student_leave(student_id)
    //       - Decrement counter for students in the room
    pthread_mutex_lock(&lock);
    student_leave(student_id);
    current_students--;
    pthread_cond_broadcast(&room_cond);
    pthread_mutex_unlock(&lock);
}


void handle_tutor(int tutor_id) {
    // TODO: Wait until either:
    //       - The tutor's service has started OR
    //       - The study room is closed
    //
    //       Use a condition variable inside a loop while holding the mutex.
    //       If the room is closed and the tutor was not activated, leave immediately.

    pthread_mutex_lock(&lock);
    while (!activated_tutors[tutor_id] && !room_closed) {
        pthread_cond_wait(&cond, &lock);
    }

    if (!activated_tutors[tutor_id]) {
        tutor_leave(tutor_id);
        pthread_mutex_unlock(&lock);
        return;
    }
    
    // TODO: Wait until the room has space for tutors (max 2 at a time)
    //       - Use condition variable and a counter for current tutors in the room.

    while (current_tutors >= MAX_TUTORS_IN_ROOM) {
        pthread_cond_wait(&room_cond, &lock);
    }
    
    // TODO: Enter the room
    //       - Call tutor_enter(tutor_id)
    //       - Increment counter for tutors in the room
    tutor_enter(tutor_id);
    current_tutors++;

    // TODO: Help up to 2 students (or fewer if not enough activated students remain)
    //       - Match with waiting students
    //       - For each student helped:
    //           - Call tutor_helps_student(tutor_id, student_id)
    //           - Notify student that they have been helped

    int helped_count = 0;

    while (helped_count < 2) {
        int found_student = 0;
        for (int sid = 0; sid < MAX_STUDENTS; sid++) {
            if (activated_students[sid] && entered[sid] && !helped[sid]) {
                helped[sid] = 1;
                tutor_helps_student(tutor_id, sid);
                pthread_cond_signal(&student_ready[sid]);
                helped_count++;
                found_student = 1;
                break;
            }
        }

        if (!found_student) {
            if (room_closed) {
                break;
            }
            pthread_cond_wait(&cond, &lock); // Wait for student to be activated/enter
        }
    }
    
    // TODO: Leave the room
    //       - Call tutor_leave(tutor_id)
    //       - Decrement counter for tutors in the room
    tutor_leave(tutor_id);
    current_tutors--;
    pthread_cond_broadcast(&room_cond);
    pthread_mutex_unlock(&lock);
}


void student_reservation_start(int student_id) {
    // TODO: Notify the students that their reservation has started
    pthread_mutex_lock(&lock);
    activated_students[student_id] = 1;
    pthread_cond_broadcast(&cond); // Wake any students or tutors waiting
    pthread_mutex_unlock(&lock);
}

void tutor_service_start(int tutor_id) {
    // TODO: Notify the tutor that their service has started
    pthread_mutex_lock(&lock);
    activated_tutors[tutor_id] = 1;
    pthread_cond_broadcast(&cond); // Wake tutors
    pthread_mutex_unlock(&lock);
}

void close_study_room(void) {
    // TODO: Notify all non-activated students and tutors that room is closing
    pthread_mutex_lock(&lock);
    room_closed = 1;
    pthread_cond_broadcast(&cond);      // Wake anyone waiting for activation
    pthread_cond_broadcast(&room_cond); // Wake anyone waiting for space
    pthread_mutex_unlock(&lock);

    // Delay to catch any straggling threads and wake again
    //usleep(1000);
    //pthread_mutex_lock(&lock);
    //pthread_cond_broadcast(&cond);
    //pthread_mutex_unlock(&lock);
}

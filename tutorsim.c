#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "testlib.h"

// Example shared variable
int students_waiting = 0;

// Example mutex and condition variable
pthread_mutex_t lock;
pthread_cond_t cond;

void init_tutor_sim(void) {
    // Initialize synchronization primitives
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);
}

void destroy_tutor_sim(void) {
    // Destroy synchronization primitives
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
}

void handle_student(int student_id) {
    // TODO: Wait until either:
    //       - The student's reservation has started OR
    //       - The study room is closed
    //
    //       Use a condition variable inside a loop while holding the mutex.
    //       If the room is closed and the student was not activated, leave immediately.

    // TODO: Wait until the room has space for students (max 4 at a time)
    //       - Use condition variable and a counter for current students in the room.

    // TODO: Enter the room
    //       - Call student_enter(student_id)
    //       - Increment counter for students in the room

    // TODO: Wait until a tutor helps this student
    //       - Use a mechanism to match tutor and student
    //       - You may use a queue or per-student flag/signaling

    // TODO: Leave the room
    //       - Call student_leave(student_id)
    //       - Decrement counter for students in the room
}


void handle_tutor(int tutor_id) {
    // TODO: Wait until either:
    //       - The tutor's service has started OR
    //       - The study room is closed
    //
    //       Use a condition variable inside a loop while holding the mutex.
    //       If the room is closed and the tutor was not activated, leave immediately.

    // TODO: Wait until the room has space for tutors (max 2 at a time)
    //       - Use condition variable and a counter for current tutors in the room.

    // TODO: Enter the room
    //       - Call tutor_enter(tutor_id)
    //       - Increment counter for tutors in the room

    // TODO: Help up to 2 students (or fewer if not enough activated students remain)
    //       - Match with waiting students
    //       - For each student helped:
    //           - Call tutor_helps_student(tutor_id, student_id)
    //           - Notify student that they have been helped

    // TODO: Leave the room
    //       - Call tutor_leave(tutor_id)
    //       - Decrement counter for tutors in the room
}


void student_reservation_start(int student_id) {
    // TODO: Notify the students that their reservation has started
}

void tutor_service_start(int tutor_id) {
    // TODO: Notify the tutor that their service has started
}

void close_study_room(void) {
    // TODO: Notify all non-activated students and tutors that room is closing
}

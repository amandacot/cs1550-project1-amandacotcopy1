
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "log.h"

#define MAX_STUDENTS 100
#define MAX_TUTORS 10
#define MAX_STUDENTS_IN_ROOM 4
#define MAX_TUTORS_IN_ROOM 2

static bool student_entered[MAX_STUDENTS];
static bool student_left[MAX_STUDENTS];
static bool student_helped[MAX_STUDENTS];
static int tutor_help_count[MAX_TUTORS];
static bool tutor_entered[MAX_TUTORS];
static bool tutor_left[MAX_TUTORS];
static bool student_activated[MAX_STUDENTS];
static bool tutor_activated[MAX_TUTORS];

static int current_students = 0;
static int current_tutors = 0;
static bool room_closed = false;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void mark_student_activated(int student_id) {
    pthread_mutex_lock(&lock);
    student_activated[student_id] = true;
    info("Student %d started reservation\n", student_id);
    pthread_mutex_unlock(&lock);
}

void mark_tutor_activated(int tutor_id) {
    pthread_mutex_lock(&lock);
    tutor_activated[tutor_id] = true;
    info("Tutor %d started service\n", tutor_id);
    pthread_mutex_unlock(&lock);
}

void student_enter(int student_id) {
    pthread_mutex_lock(&lock);
    if (!student_activated[student_id]) {
        error("Student %d entered before activation\n", student_id);
    }
    if (student_entered[student_id]) {
        error("Student %d entered more than once\n", student_id);
    }
    if (current_students >= MAX_STUDENTS_IN_ROOM) {
        error("Student %d entered when room at max capacity\n", student_id);
    }
    student_entered[student_id] = true;
    current_students++;
    info("Student %d entered\n", student_id);
    pthread_mutex_unlock(&lock);
}

void student_leave(int student_id) {
    pthread_mutex_lock(&lock);
    if (!student_entered[student_id]) {
        if (!room_closed || student_activated[student_id]) {
            error("Student %d left before entering\n", student_id);
        } else {
            info("Student %d left\n", student_id);
            student_left[student_id] = true;
            pthread_mutex_unlock(&lock);
            return;
        }
    }
    if (!student_helped[student_id]) {
        error("Student %d left without being helped\n", student_id);
    }
    if (student_left[student_id]) {
        error("Student %d left more than once\n", student_id);
    }
    student_left[student_id] = true;
    current_students--;
    info("Student %d left\n", student_id);
    pthread_mutex_unlock(&lock);
}

void tutor_enter(int tutor_id) {
    pthread_mutex_lock(&lock);
    if (!tutor_activated[tutor_id]) {
        error("Tutor %d entered before activation\n", tutor_id);
    }
    if (tutor_entered[tutor_id]) {
        error("Tutor %d entered more than once\n", tutor_id);
    }
    if (current_tutors >= MAX_TUTORS_IN_ROOM) {
        error("Tutor %d entered when room at max tutor capacity\n", tutor_id);
    }
    tutor_entered[tutor_id] = true;
    current_tutors++;
    info("Tutor %d entered\n", tutor_id);
    pthread_mutex_unlock(&lock);
}

void tutor_leave(int tutor_id) {
    pthread_mutex_lock(&lock);
    if (!tutor_entered[tutor_id]) {
        if (!room_closed || tutor_activated[tutor_id]) {
            error("Tutor %d left before entering\n", tutor_id);
        } else {
            info("Tutor %d left\n", tutor_id);
            tutor_left[tutor_id] = true;
            pthread_mutex_unlock(&lock);
            return;
        }
    }
    if (tutor_left[tutor_id]) {
        error("Tutor %d left more than once\n", tutor_id);
    }
    tutor_left[tutor_id] = true;
    current_tutors--;
    info("Tutor %d left\n", tutor_id);
    pthread_mutex_unlock(&lock);
}

void tutor_helps_student(int tutor_id, int student_id) {
    pthread_mutex_lock(&lock);
    if (!tutor_entered[tutor_id]) {
        error("Tutor %d helped a student before entering\n", tutor_id);
    }
    if (!student_entered[student_id]) {
        error("Student %d was helped before entering\n", student_id);
    }
    if (student_helped[student_id]) {
        error("Student %d was helped more than once\n", student_id);
    }
    if (tutor_help_count[tutor_id] >= 2) {
        error("Tutor %d helped more than two students\n", tutor_id);
    }
    student_helped[student_id] = true;
    tutor_help_count[tutor_id]++;
    info("Tutor %d helped Student %d\n", tutor_id, student_id);
    pthread_mutex_unlock(&lock);
}

void testlib_close_study_room(void) {
    pthread_mutex_lock(&lock);
    room_closed = true;
    info("Study room closed\n");
    pthread_mutex_unlock(&lock);
}

void check_no_hanging_events(void) {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (student_activated[i]) {
            if (!student_entered[i]) {
                error("Activated student %d never entered\n", i);
            }
            if (!student_helped[i] || !student_left[i]) {
                error("Activated student %d did not complete lifecycle properly\n", i);
            }
        } else {
            // Not activated → must NOT enter or be helped, but MUST leave after close
            if (student_entered[i] || student_helped[i]) {
                error("Non-activated student %d participated unexpectedly\n", i);
            }
            if (!student_left[i]) {
                error("Non-activated student %d did not leave after room close\n", i);
            }
        }
    }

    for (int i = 0; i < MAX_TUTORS; i++) {
        if (tutor_activated[i]) {
            if (!tutor_entered[i]) {
                error("Activated tutor %d never entered\n", i);
            }
            if (!tutor_left[i]) {
                error("Activated tutor %d did not leave\n", i);
            }
        } else {
            // Not activated → must NOT enter, but MUST leave after close
            if (tutor_entered[i]) {
                error("Non-activated tutor %d entered unexpectedly\n", i);
            }
            if (!tutor_left[i]) {
                error("Non-activated tutor %d did not leave after room close\n", i);
            }
        }
    }

    info("Checked for end of simulation state\n");
    pthread_mutex_unlock(&lock);
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "testlib.h"

// External simulation functions
extern void handle_student(int student_id);
extern void handle_tutor(int tutor_id);
extern void student_reservation_start(int student_id);
extern void tutor_service_start(int tutor_id);
extern void close_study_room(void);
extern void init_tutor_sim(void);
extern void destroy_tutor_sim(void);

#define MAX_STUDENTS 100
#define MAX_TUTORS 10

void *run_student(void *arg)
{
    int id = *(int *)arg;
    handle_student(id);
    return NULL;
}

void *run_tutor(void *arg)
{
    int id = *(int *)arg;
    handle_tutor(id);
    return NULL;
}

void parse_ids(const char *env, int *ids, int *count)
{
    *count = 0;
    if (!env)
        return;
    char *copy = strdup(env);
    char *token = strtok(copy, ",");
    while (token && *count < MAX_STUDENTS)
    {
        ids[(*count)++] = atoi(token);
        token = strtok(NULL, ",");
    }
    free(copy);
}

void cleanup()
{
    check_no_hanging_events();
    destroy_tutor_sim();
}

int main()
{
    pthread_t students[MAX_STUDENTS];
    pthread_t tutors[MAX_TUTORS];

    init_tutor_sim();
    atexit(cleanup);

    for (int i = 0; i < MAX_STUDENTS; ++i)
    {
        int *sid = malloc(sizeof(int));
        *sid = i;
        pthread_create(&students[i], NULL, run_student, sid);
    }

    for (int i = 0; i < MAX_TUTORS; ++i)
    {
        int *tid = malloc(sizeof(int));
        *tid = i;
        pthread_create(&tutors[i], NULL, run_tutor, tid);
    }

    sleep(1);

    char *sequence_env = getenv("ACTIVATE_SEQUENCE");
    if (!sequence_env)
    {
        fprintf(stderr, "Missing ACTIVATE_SEQUENCE environment variable.\n");
        exit(1);
    }

    char *token = strtok(sequence_env, ",");
    while (token)
    {
        if (token[0] == 'S')
        {
            int sid = atoi(&token[1]);
            mark_student_activated(sid);
            student_reservation_start(sid);
        }
        else if (token[0] == 'T')
        {
            int tid = atoi(&token[1]);
            mark_tutor_activated(tid);
            tutor_service_start(tid);            
        }
        else
        {
            fprintf(stderr, "Invalid token in ACTIVATE_SEQUENCE: %s\n", token);
        }
        token = strtok(NULL, ",");
    }

    sleep(1);
    testlib_close_study_room();
    close_study_room();

    for (int i = 0; i < MAX_STUDENTS; ++i)
        pthread_join(students[i], NULL);
    for (int i = 0; i < MAX_TUTORS; ++i)
        pthread_join(tutors[i], NULL);

    return 0;
}

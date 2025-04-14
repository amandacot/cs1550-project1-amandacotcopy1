#ifndef TESTLIB_H
#define TESTLIB_H

// Called by simulation logic
void student_enter(int student_id);
void student_leave(int student_id);
void tutor_enter(int tutor_id);
void tutor_leave(int tutor_id);
void tutor_helps_student(int tutor_id, int student_id);

// Called by driver to mark activation
void mark_student_activated(int student_id);
void mark_tutor_activated(int tutor_id);

// Called by driver to simulate closing the room
void testlib_close_study_room(void);

// Called at the end to check lifecycle correctness
void check_no_hanging_events(void);

#endif // TESTLIB_H

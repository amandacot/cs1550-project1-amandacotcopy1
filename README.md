# CS 1550: Project 1 — Tutor Room Synchronization Simulation (16% of the grade) 🎓📚💻

## Overview 🧠🧵🔒
In this project, you will help write a C program that simulates the interaction between students and tutors in a shared study room using POSIX threads (`pthreads`). The goal is to model concurrent behavior and enforce proper synchronization based on shared rules. 🎯🪢🧮

Your implementation must avoid deadlocks and race conditions and demonstrate proper coordination between multiple types of threads. 🧩⚠️🚦

## Actors 👨‍🎓👩‍🏫🚪
There are two types of threads:

- **Students**: Attempt to enter the study room if they have a valid reservation.
- **Tutors**: Enter to help students.

There are a total of **100 students** and **10 tutors** in the system. 🧑‍🎓🧑‍🏫🔢

## Rules and Constraints 📏📌📚

### 1. Reservations 📅🔔🪪
- Students are activated by reservation events.
- A student may enter the study room only after their reservation has started.
- The reservation start function is invoked by the driver.

### 2. Student Entry and Exit 🚶‍♂️🎓🏁
- A student may only enter *after* their reservation is active.
- A student may only leave *after* being helped by a tutor.
- A student may only be helped **once**.
- At most **4 students** can be in the room at the same time. 🔢🚪👥

### 3. Tutor Behavior 👨‍🏫✅📘
- A tutor may only help students while marked as "in service".
- The tutor activation happens when the driver calls `tutor_service_start()`.
- At most **2 tutors** can be in the room at the same time. 🧑‍🏫🧑‍🏫🚪
- Each tutor must leave after helping **two** students unless there are not enough activated students remaining. 🧑‍🏫🧑‍🏫✅

### 4. Validation ✅📣🛠️

Unlike traditional output-based checking, this project uses **function-based event validation**. Your code must call specific functions defined in `testlib.h` to report events (e.g., student entering, tutor helping). These functions automatically validate correctness and report errors if your simulation violates any of the rules. ✅🔎📋

You **must call** the following validator functions to track simulation correctness:

```c
void student_enter(int student_id);
void student_leave(int student_id);
void tutor_enter(int tutor_id);
void tutor_leave(int tutor_id);
void tutor_helps_student(int tutor_id, int student_id);
```

**When to call each function:**
- `student_enter` — when a student successfully enters the study room.
- `student_leave` — after a student has been helped and leaves the room.
- `tutor_enter` — when a tutor begins helping students.
- `tutor_leave` — after the tutor finishes helping (normally two students).
- `tutor_helps_student` — when a tutor helps a student.

### 5. Termination and Closing the Room 🔚🧹🚪
The driver will invoke a room closure event by calling:

```c
void close_study_room(void);
```

After this call:
- All **non-activated** students and tutors must exit immediately.
- **Activated** students and tutors (even the ones who have not entered the room yet) must follow the usual rules:
  - Students may only leave after being helped.
  - Tutors must help two students before leaving, unless not enough students remain.

At the very end of the simulation, the driver invokes:

```c
void check_no_hanging_events(void);
```

This checks that:
- All activated students have been helped and left.
- All activated tutors have entered the room and left.

## Starter Files 📂📄📦
- `driver.c` — The main driver program that simulates reservation and service events.
- `tutorsim.c` — This is the **only** file you will modify. You should implement:
  - `void init_tutor_sim();`
  - `destroy_tutor_sim();`
  - `void handle_student(int student_id);`
  - `void handle_tutor(int tutor_id);`
  - `void student_reservation_start(int student_id);`
  - `void tutor_service_start(int tutor_id);`
  - `void close_study_room(void);`
- `testlib.h` / `testlib.c` — Interface and implementation for validation functions.
- `Makefile` — Compiles your simulation with `gcc -pthread`.
- `test_harness.sh` — Runs your simulation by configuring the number and order of student/tutor activations, and issues the close room event.
- `README-threading.md` — Guide on using pthreads.

## Compilation 🛠️📥⚙️
To compile your simulation:

```bash
make
```

This will produce an executable named `tutor_sim`. 🧾🚀📎

## Running the Simulation 🎮🧑‍💻📈

```bash
ACTIVATE_SEQUENCE=S1,S2,T0,S3,T1 ./tutor_sim
```

You may redirect output if needed:

```bash
ACTIVATE_SEQUENCE=S1,S2,T0,S3,T1 ./tutor_sim > output.txt
```

To customize which students and tutors are activated, and the order in which they are activated, you can use the ACTIVATE_SEQUENCE environment variable.

✅ Example with Custom Activation Order:
```bash
ACTIVATE_SEQUENCE=S1,S2,T0,S3,T1 ./tutor_sim
```
This activates:
Student 1
Student 2
Tutor 0
Student 3
Tutor 1
...in that exact order.

🧠 Format:
Use S<number> for students (e.g., S5 = student 5)

Use T<number> for tutors (e.g., T2 = tutor 2)

Separate entries with commas `,`.

You can activate any subset of the 100 students (S0–S99) and 10 tutors (T0–T9)

## Testing 🧪🔍🔧
Run the harness with:

```bash
bash test_harness.sh
```

This harness configures specific students and tutors to activate and issues a `close_study_room()` event. It checks simulation behavior using the validation functions.

## What You Should Implement 🧑‍💻🧵🛠️
You are responsible for writing the core simulation logic inside `tutorsim.c`:

- `init_tutor_sim()` — initializes any shared variables, mutexes, or condition variables. This function will be called by the driver before any threads are created.
- `destroy_tutor_sim()` — releases any resources and destroys synchronization primitives. This function will be called by the driver at the end of the simulation.
- `handle_student(int student_id)` — handles the lifecycle of an individual student.
- `handle_tutor(int tutor_id)` — handles the lifecycle of an individual tutor.
- `student_reservation_start`, `tutor_service_start`, and `close_study_room` — invoked by the driver thread.

Refer to the [README on thread synchronization](README-threading.md) for additional design guidance and concurrency advice. 🧠💡🔗


## Deliverables 📬📂📤
You must submit:
- **Only** `tutorsim.c` with your implementation.

**Do not submit or modify** `driver.c`, helper files, or any of the validation libraries.

## Debugging and Memory Analysis 🐞🧠🧰

### Using GDB for Multi-threaded Debugging
To debug your multi-threaded program with GDB:

```bash
gdb ./tutor_sim
```

Useful commands in GDB:
- `run` — Start the program.
- `break function_name` — Set a breakpoint.
- `info threads` — List all threads.
- `thread n` — Switch to thread number n.
- `bt` — Backtrace to see function call history.

You can use `thread apply all bt` to get backtraces from all threads.

To influence the program execution, you can set its environment variables before the program starts:

```
(gdb) set env ACTIVATE_SEQUENCE=S1,S2,T0,S3,T1
(gdb) r
```
To investigate a deadlock situation, you can attach `gdb` to the process by first grabbing the process id then running `gdb` and using the `attach` command. 
0. Set `/proc/sys/kernel/yama/ptrace_scope` to 0 using the following commands: `sudo bash` then `echo 0 > /proc/sys/kernel/yama/ptrace_scope`
1. Press `CTRL+Z` to put the deadlocked program in the background.
2. run `ps`
3. Identify the process ID for `tutor_sim`
4. Run `gdb ./tutor_sim`
5. Type `attach <PID>` there `PID` is the process ID you got in step 
You can then run `thread apply all bt` in gdb to show where each thread is waiting. You’ll want to examine the code lines in `tutorsim.c`.

To dig deeper:

1.  Run `thr 2` to switch to a particular thread (e.g., thread `2`).
2.  Run `bt` to get a stack trace, and `frame 5` (to jump to stack frame `5`, for example) to jump to the point in `tutorsim.c` where you called some blocking construct.

### Using Valgrind for Memory and Synchronization Issues
Valgrind helps catch memory errors and race conditions:

```bash
valgrind --tool=helgrind ./tutor_sim
```

Or to check for memory leaks:

```bash
valgrind --leak-check=full ./tutor_sim
```

Watch for:
- Use of uninitialized memory.
- Memory leaks.
- Data races or mutex/condition variable misuse.

---
## Academic Integrity 👩‍⚖️🚫📜
This is an individual assignment. All submitted code must be your own. Please refer to the Academic Integrity policy in the Syllabus on Canvas for more details.

---
Good luck! 🌟👍📘

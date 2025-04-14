CC = gcc
CFLAGS = -Wall -pthread -g
OBJS = driver.o tutorsim.o testlib.o log.o

all: tutor_sim

tutor_sim: $(OBJS)
	$(CC) $(CFLAGS) -o tutor_sim $(OBJS)

driver.o: driver.c testlib.h
	$(CC) $(CFLAGS) -c driver.c

tutorsim.o: tutorsim.c testlib.h
	$(CC) $(CFLAGS) -c tutorsim.c

testlib.o: testlib.c testlib.h
	$(CC) $(CFLAGS) -c testlib.c

log.o: log.c log.h
	$(CC) $(CFLAGS) -c log.c

clean:
	rm -f *.o tutor_sim

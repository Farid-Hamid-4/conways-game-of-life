all: gol_data gol_task

gol_data: gol_data.o helpers.o
	gcc -Wall -pedantic -g -pthread gol_data.o helpers.o -o gol_data

gol_data.o: gol_data.c
	gcc -Wall -pedantic -g -pthread -c gol_data.c -o gol_data.o

gol_task: gol_task.o helpers.o queue.o
	gcc -Wall -pedantic -g -pthread gol_task.o helpers.o queue.o -o gol_task

gol_task.o: gol_task.c
	gcc -Wall -pedantic -g -pthread -c gol_task.c -o gol_task.o

helpers.o: helpers.c
	gcc -Wall -pedantic -c helpers.c -o helpers.o

queue.o: queue.c
	gcc -Wall -pedantic -c queue.c -o queue.o

clean:
	rm *.o gol_data gol_task
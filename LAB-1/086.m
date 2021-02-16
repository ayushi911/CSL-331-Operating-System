all: 
	gcc 086-task1.c -o task1
	gcc 086-task2.c -o task2
	gcc 086-task3.c -o task3
	gcc task2-child.c -o child

clean: 
	rm -rf *.o task1 task2 task3 child *.txt *.bin

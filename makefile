all:task1 task2 task3 task4a task4b

LineParser.o:LineParser.c
	gcc -g -m32 -c -o LineParser.o LineParser.c

task1.o:task1.c 
	gcc -g -m32 -c -o task1.o task1.c
task1: task1.o LineParser.o
	gcc -m32 -o task1 task1.o LineParser.o

task2.o:task2.c 
	gcc -g -m32 -c -o task2.o task2.c
task2: task2.o
	gcc -m32 -o task2 task2.o

task3.o:task3.c 
	gcc -g -m32 -c -o task3.o task3.c
task3: task3.o LineParser.o
	gcc -m32 -o task3 task3.o LineParser.o

task4a.o:task4a.c 
	gcc -g -m32 -c -o task4a.o task4a.c
task4a: task4a.o LineParser.o
	gcc -m32 -o task4a task4a.o LineParser.o

task4b.o:task4b.c 
	gcc -g -m32 -c -o task4b.o task4b.c
task4b: task4b.o LineParser.o
	gcc -m32 -o task4b task4b.o LineParser.o

.PHONY: clean
clean:
	 rm -rf ./*.o task1
	 rm -rf ./*.o task2
	 rm -rf ./*.o task3
	 rm -rf ./*.o task4a
	 rm -rf ./*.o task4b

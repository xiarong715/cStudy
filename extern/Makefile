extern: main.o test.o
	gcc -o extern main.o test.o
main.o: main.c
	gcc -o main.o -c main.c
test.o: test.c
	gcc -o test.o -c test.c
clean:
	rm -rf main.o test.o extern
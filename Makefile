all: reso
reso: reso.c
	gcc -g -o reso reso.c
clean:
	rm reso

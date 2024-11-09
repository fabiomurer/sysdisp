# -Wall -Werror -Wpedantic -Wextra

run: all
	LD_PRELOAD=./bootstrapcode.so ./simpleprog

all: simpleprog bootstrapcode.so childprog

bootstrapcode.so: bootstrapcode.c
	gcc -FPIC -shared bootstrapcode.c -o bootstrapcode.so

simpleprog: simpleprog.c
	gcc simpleprog.c -o simpleprog

childprog: childprog.c
	gcc childprog.c -o childprog

.PHONY: clean

clean:
	rm simpleprog bootstrapcode.so childprog
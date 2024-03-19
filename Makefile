hello: 06_test/main.c
		gcc -o $@ $< -luring

all: regular_cat cat_liburing cp_liburing webserver_liburing hello

.PHONY: clean

clean:
	rm -f regular_cat cat_uring cat_liburing cp_liburing webserver_liburing hello

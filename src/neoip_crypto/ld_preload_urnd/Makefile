# very raw Makefile to build libnss_neoip package via epm

static_build:	redir_open_static.o

redir_open_dynamic.o: redir_open_dynamic.c
	gcc -fPIC -c -o redir_open_dynamic.o redir_open_dynamic.c

redir_open_dynamic.so: redir_open_dynamic.o
	gcc -shared -o redir_open_dynamic.so redir_open_dynamic.o -ldl

redir_open_static.o: redir_open_static.c
	gcc -fPIC -c -o redir_open_static.o redir_open_static.c


test_dynamic: redir_open_dynamic.so
	LD_PRELOAD="./redir_open_dynamic.so" dd if=/dev/urandom count=2


test_static: redir_open_static.o
	gcc -fPIC -c -o test_static.o test_static.c
	gcc redir_open_static.o -lc test_static.o -o test_static
	./test_static

clean:
	rm -f *.o *.so test_static


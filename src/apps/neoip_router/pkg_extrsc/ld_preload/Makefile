# very raw Makefile to build libnss_neoip package via epm

redir_open.o: redir_open.c
	gcc -fPIC -c -o redir_open.o redir_open.c

redir_open.so: redir_open.o
	gcc -shared -o redir_open.so redir_open.o -ldl

test: redir_open.so
	LD_PRELOAD="./redir_open.so" dd if=/dev/urandom count=2

clean:
	rm -f *.o *.so

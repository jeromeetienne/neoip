# very raw Makefile to compile libnss_neoip.so.2 to work with gnu nss


CFLAGS+=-Wall
CFLAGS+=-Werror
CFLAGS+=-O3

libnss_neoip.so.2: libnss_neoip.o
	gcc -shared -o $@ -Wl,-soname,$@ libnss_neoip.o
	 
libnss_neoip.o: libnss_neoip.c libnss_neoip_hostent.c libnss_neoip_tools.c libnss_neoip_dir.c \
					libnss_neoip_socket.c libnss_neoip_err.c
clean:
	rm -f libnss_neoip.so.2
	rm -f libnss_neoip.o

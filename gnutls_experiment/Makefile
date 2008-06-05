# This is a raw makefile to compile the various experimentation


all: main_client1 ex-serv-anon ex-client1

main_client1: main_client1.c tcp.c ex-session-info.c ex-x509-info.c
	cc -Wall main_client1.c tcp.c ex-session-info.c ex-x509-info.c  -lgnutls -o main_client1

ex-serv-anon: ex-serv-anon.c
	cc -Wall ex-serv-anon.c -lgnutls -o ex-serv-anon

ex-client1: ex-client1.c
	cc -Wall ex-client1.c tcp.c -lgnutls -o ex-client1

SUBDIRS		= mylib1 mylib2

bin_PROGRAMS	= myprog

myprog_SOURCES	= main.c

myprog_LDADD	= mylib2/libmylib2-1.0.la mylib1/libmylib1-1.0.la 


# jme- to workaround a bug in autotools - without this, it display 
# config.status: WARNING: Makefile.in seems to ignore the --datarootdir setting
datarootdir = @datarootdir@

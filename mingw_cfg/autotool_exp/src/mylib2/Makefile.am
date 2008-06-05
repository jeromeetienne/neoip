lib_LTLIBRARIES		= libmylib2-1.0.la
libmylib2_1_0_la_SOURCES= mylib2_1.c mylib2_2.c

libmylib2_1_0_la_LDFLAGS+= -no-undefined --export-all-symbols -L../mylib1/.libs/

libmylib2_1_0_la_LIBADD	= -lmylib1-1.0

# jme- to workaround a bug in autotools - without this, it display 
# config.status: WARNING: Makefile.in seems to ignore the --datarootdir setting
datarootdir = @datarootdir@

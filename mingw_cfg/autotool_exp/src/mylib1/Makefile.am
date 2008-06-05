lib_LTLIBRARIES		= libmylib1-1.0.la
libmylib1_1_0_la_SOURCES= mylib1_1.c mylib1_2.c


libmylib1_1_0_la_LDFLAGS+= -no-undefined --export-all-symbols
			

#	$(CC) -shared -o $@				\
#		-Wl,--out-implib=$@.a			\
#		-Wl,--export-all-symbols		\
#		-Wl,--enable-auto-import		\
#		-Wl,--whole-archive mydll.o		\
#		-L. -Wl,--no-whole-archive otherdll.a

# jme- to workaround a bug in autotools - without this, it display 
# config.status: WARNING: Makefile.in seems to ignore the --datarootdir setting
datarootdir = @datarootdir@


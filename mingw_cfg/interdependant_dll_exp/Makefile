CC	:= i586-mingw32msvc-gcc
AR	:= i586-mingw32msvc-ar
RANLIB	:= i586-mingw32msvc-ranlib

all: myprog.exe

mydll.a: mydll.o
	$(AR) rcs $@ $<
	$(RANLIB) $@

mydll.dll: mydll.o otherdll.a
	$(CC) -shared -o $@				\
		-Wl,--out-implib=$@.a			\
		-Wl,--export-all-symbols		\
		-Wl,--enable-auto-import		\
		-Wl,--whole-archive mydll.o		\
		-Wl,--no-whole-archive otherdll.a


otherdll.a: otherdll.o
	$(AR) rcs $@ $<
	$(RANLIB) $@

otherdll.dll: otherdll.o mydll.a
	$(CC) -shared -o $@				\
		-Wl,--out-implib=$@.a			\
		-Wl,--export-all-symbols		\
		-Wl,--enable-auto-import		\
		-Wl,--whole-archive otherdll.o		\
		-Wl,--no-whole-archive mydll.a

myprog.exe: myprog.o mydll.dll otherdll.dll
	$(CC) -o myprog.exe myprog.o mydll.dll.a otherdll.dll.a
	
clean:
	$(RM) mydll.o mydll.dll mydll.dll.a mydll.a
	$(RM) otherdll.o otherdll.dll otherdll.dll.a otherdll.a
	$(RM) myprog.o myprog.exe
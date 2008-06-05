ifndef NEOIP_TARGET
	#NEOIP_TARGET	= linux
	NEOIP_TARGET	= win32
endif

ifeq ($(NEOIP_TARGET), win32)
	CC=i586-mingw32msvc-cc
endif


# i dunno why im not confortable with having all the exe in a separate dir
# likely because i will have to type the path... i dunno
# - maybe to use the ./m to do the option more easily

# in src/build_linux do make -f ../Makefile NEOIP_TARGET=linux
# in src/build_win32 do make -f ../Makefile NEOIP_TARGET=win32
# only the VPATH and the rules are enougth assuming the 2 above commands
VPATH=../src
%.o: %.c
	$(CC) -c -o $@ $<
# mirror the src directory structure in the build directory
# - this is required in order to later write the .o in them
# - so it is required to be uptodate and done before compilation
mirror_build_dir:
	find ../src/* -type d| sed 's/^\.\.\/src\///g' | xargs mkdir -p

#
# from here normal makefile stuff
#

all: myapps

myapps: main.o subdir/subfile.o
	$(CC) $^ -o $@ 
	
clean:
	find . -name "*.o" | xargs rm
	rm -f myapps

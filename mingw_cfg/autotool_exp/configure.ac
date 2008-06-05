# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)

# Register ourselves to autoconf, giving the main source file
AC_INIT(src/main.c)

# Init Automake, giving the program name and version. More parameters (author and author's email) are optional
AM_INIT_AUTOMAKE([1.9 foreign])

# Enable maintainer mode (debugging flags etc)
#AM_MAINTAINER_MODE

# - to avoid to have a LOT of -D in my compile line
AM_CONFIG_HEADER(config.h)

# Check whether we got a good C compiler. Variable "CC" will be defined and expanded in the .in files
AC_PROG_CC

# check if there is a libtool
AM_PROG_LIBTOOL

# Here we tell the configure script which files to *create*, so we leave out the .in part
AC_CONFIG_FILES([
	Makefile		\
	src/mylib1/Makefile	\
	src/mylib2/Makefile	\
	src/Makefile
])

AC_OUTPUT
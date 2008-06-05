# If you were going to use this Makefile as an example to build another,
# you would want to understand the following bits:
#
# PROJ is the master project list; put your project name there
# proj_DIR is where the source for this project lives
# proj_BIN is a list of binaries to build
# proj_OBJ is the list of objects that makes up that binary
# proj_CFLAGS are the optional extra CFLAGS to use when building
# proj_clean is the rule that's called to cleanup
# 
# Was that so hard?

# Add 'yavipind' to the global project list, so that it gets built
PROJ += yavipind

# Where this source lives
yavipind_DIR = src

# The list of binaries to build for this project
yavipind_BIN := $(yavipind_DIR)/yavipind

# The list of man page
yavipind_MAN := $(yavipind_DIR)/yavipind.8

# Our list of object files, with some magic to add the local path poop
yavipind_SRC := $(wildcard $(yavipind_DIR)/*.c)
yavipind_OBJ := $(yavipind_SRC:.c=.o)

# We need to include glib and yavipind things, so do so.  Since glib is a
# common thing to want, the top-level Makefile provides GLIB_INC and
# GLIB_LIB for us
yavipind_CFLAGS := $(GLIB_INC) -Wall

yavipind_LIBS := -L/usr/local/ssl/lib $(GLIB_LIB) -lcrypto -lz

# Teach it how to build 'yavipind'
$(yavipind_BIN): $(yavipind_OBJ)
	$(CC) -o $@ $^  $(yavipind_LIBS)

yavipind_clean:
	rm -f $(yavipind_OBJ) $(yavipind_BIN)

yavipind_install:
	$(INSTALL) $(yavipind_BIN) $(SBINDIR)
	$(INSTALL) $(yavipind_MAN) $(MANDIR)/man8


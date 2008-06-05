CC := gcc
INSTALL := /usr/bin/install -c
BINDIR := /usr/local/${exec_prefix}/bin
SBINDIR := /usr/local/${exec_prefix}/sbin
MANDIR := /usr/local/${prefix}/man
LIBDIR := /usr/local/${exec_prefix}/lib
GLIB_INC := $(shell glib-config --cflags)
GLIB_LIB := $(shell glib-config --libs)

default: all

# Include the various module definitions
include src/Makefile.inc

# Look for include files in each of the modules (FIXME: this is a hack!)
CFLAGS = -g -O2
#LDFLAGS =  -lcrypto -lz -lglib 
#CFLAGS += -I src

# Include the C include Deps
#TEMP := $(foreach tmp,$(PROJ),$($(tmp)_OBJ))
#DEPS := $(TEMP:.o=.d)
#include $(DEPS)

all: $(foreach tmp,$(PROJ),$($(tmp)_BIN))

# Build files with project-specific CFLAGS
%.o: %.c
	$(CC) $(CFLAGS) $(yavipind_CFLAGS) -c $< -o $@

# Calculate C include deps
%.d: %.c
	scripts/depend.sh $(CFLAGS) $< > $@

# Special clean rule to let projects specify their own cleanup
clean: $(foreach tmp,$(PROJ),$(tmp)_clean)
	rm -f $(DEPS)

distclean: clean
	rm -f config.cache config.log config.status include/autoconf.h \
	Makefile $(foreach tmp,$(PROJ),$($(tmp)_DIR)/Makefile.inc)

# Likewise for install
.PHONY: install clean
pre-install:
	mkdir -p $(BINDIR)
	mkdir -p $(MANDIR)/man8

install: pre-install $(foreach tmp,$(PROJ),$(tmp)_install)

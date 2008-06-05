#!/bin/bash
aclocal
autoconf

touch config.h.in

automake -a
# jme- later try with automake --foreign
# - it is supposed to avoid all the pollution with gnu specific package

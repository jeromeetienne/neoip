#!/bin/sh
# jme- helper executable to perform dns query honoring /etc/nsswitch.conf via fork()
#
# POSSIBLE IMPROVEMENT:
# - it may be made faster by a small C program, but would require a arch specific package
# - well a small exe to do that is not a real problem
# - so likely better to do it, but currently it is not a priority

# ok now the code itself
# it output the list of ip address resulting of the resolution with a '/' at the end of each address
# if the resolution fails, it output nothing
# NOTE: the protocol of the output string is rather crappy for now. may be cleaned up latter
getent hosts $1 | cut -d" " -f1 | tr '\n' '/'

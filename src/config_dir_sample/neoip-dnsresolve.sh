#!/bin/sh
# jme- helper executable to perform dns query honoring /etc/nsswitch.conf via fork()

# extract the HOSTNAME_TO_QUERY
HOSTNAME_TO_QUERY=$1

###############################################
# bash version relying on 'getent' (with specifically asking for IPv4)
###############################################
# - use ahostv4 to force IPv4
#   - using 'hosts' would fails if the hostname requested support IPv6 this will 
#     resolve in ipv6 only
#   - e.g. "getent hosts no.releases.ubuntu.com" resolve in IPv6 *only* but ok with ahostv4
#getent ahostsv4 $HOSTNAME_TO_QUERY | grep STREAM | cut -d" " -f1 | tr '\n' '/'  2>/dev/null

###############################################
# bash version relying on 'host' (with specifically asking for IPv4)
###############################################
host $HOSTNAME_TO_QUERY | grep "has address" | cut -d" " -f4 | tr '\n' '/' 2>/dev/null


###############################################
# ruby version which works with ipv6 hosts
###############################################
# NOTE: it spawn a ruby interpreter each time... so it is VERY cpu consuming
# - a small C apps may do the same with a lot less cpu. (produce a binary of 550k tho)
# - NOTE: this may require a LOT of time due to some specificity of getaddrinfo in ruby
#   - e.g. distribution.nerdtv.net takes far longer to resolve on ruby then any other means
#ruby -e "require 'socket'; begin; addr=Socket::getaddrinfo(ARGV[0],'')[0][3]; rescue; exit 1; end; puts addr; exit 0" $HOSTNAME_TO_QUERY


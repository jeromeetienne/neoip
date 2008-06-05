#! /usr/bin/env python
# jme - discover the local network interface IP which lead to the 'public internet'

import socket
import sys

# connect a remote host to get the source address of the socket
# - here checkip.dyndns.org is used to be but any public server will do
for res in socket.getaddrinfo('checkip.dyndns.org', 80, socket.AF_UNSPEC, socket.SOCK_STREAM):
    af, socktype, proto, canonname, sa = res
    try:
		s = socket.socket(af, socktype, proto)
    except socket.error, msg:
		s = None
		continue
    try:
		s.connect(sa)
    except socket.error, msg:
		s.close()
		s = None
		continue
    break

# get the local network interface address
local_netif_ip_addr=s.getsockname()[0]

# close the socket
s.close()

# display the result
print local_netif_ip_addr


#! /usr/bin/env python

# import the xmlrpc library
import xmlrpclib
import base64
import random
import math
import time

from kad_id			import kad_id
from kad_node		import kad_node
from kad_delay		import kad_delay
from kad_rec		import kad_rec
from kad_session	import kad_session

def typeof( val ):
	return val.__class__

# define some contants
realmid		= kad_id('kad realm utest');
nodeid_arr	=[]
sess_arr	=[]

# create a session with a responder
nodeid_arr.append(kad_id('nodeid ' + 'reponder'))
sess_arr.append(kad_session(realmid, nodeid_arr[0], port=9080))

# create many session in the initiator
for i in range(1, 2):
	time.sleep(0.05)
	print 'init subnode ' + str(i)
	nodeid_arr.append(kad_id("nodeid " + str(i)))
	sess_arr.append(kad_session(realmid, nodeid_arr[i], port=9081))
	
record=kad_rec([kad_id('recid'), kad_id('keyid'), 'dummy record data', kad_delay(60*2)])


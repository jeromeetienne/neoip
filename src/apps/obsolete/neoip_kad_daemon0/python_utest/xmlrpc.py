#! /usr/bin/env python

# import the xmlrpc library
import xmlrpclib
import base64

# create the context to query a server
server = xmlrpclib.ServerProxy("http://127.0.0.1:9080/neoip_lib/kad_server/xmlrpc")
#server = xmlrpclib.ServerProxy("http://127.0.0.1:9080/neoip_lib/kad_sever/xmlrpc")
# display the server
print server

# do query the server
try:
	print server.dht.publish(('RecID bla', 'KeyID bla', base64.b64encode('bonjour from dummy'), 60), "kad realm utest", 30)
#	print server.dht.delete("KeyID bla", "RecID bla", "kad realm utest",  30)
#	print server.dht.getone("KeyID bla", "kad realm utest", 30)
#	print server.dht.getall("KeyID bla", "kad realm utest", 30)
#	print server.dht.nsearch("KeyID bla", "kad realm utest", 30)

# all the rpc
#	print server.dht.ping_rpc(('NodeID', '127.0.0.1:3000'), "kad realm utest", 30)
#	print server.dht.store_rpc("kad realm utest", "KeyID:RecordID:Data bla"
#					, ('NodeID', 'ipport'), 30)
#	print server.dht.findnode_rpc("kad realm utest", "KeyID bla"
#					, ('NodeID', 'ipport'), 30)
#	print server.dht.findoneval_rpc("kad realm utest", "KeyID bla"
#					, ('NodeID', 'ipport'), 30)
#	print server.dht.findallval_rpc("kad realm utest", "KeyID bla"
#					, ('NodeID', 'ipport'), 30)
#	print server.dht.delete_rpc("kad realm utest", "KeyID bla", "RecordID bla"
#					, ('NodeID', 'ipport'), 30)
except xmlrpclib.Error, v:
	print "ERROR", v

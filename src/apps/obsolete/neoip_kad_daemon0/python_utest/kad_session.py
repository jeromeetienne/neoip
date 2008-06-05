# system import
import xmlrpclib
import base64

# local import
from kad_id		import kad_id
from kad_delay	import kad_delay
from kad_naddr	import kad_naddr
from kad_naddrc	import kad_naddrc
from kad_rec	import kad_rec

class kad_session:
	'''Class to contain a kad_session'''
	
	def __init__( self, realmid, nodeid, port = None, url = None ):
		'''Constructor'''
		# copy the realmid
		assert isinstance( realmid	, kad_id )
		self.realmid	= realmid
		# copy the nodeid
		assert isinstance( nodeid	, kad_id )
		self.nodeid		= nodeid
		# init the server from the url Xor the port parameter
		if url is not None:
			assert isinstance( url	, str )
			self.rpcserver	= xmlrpclib.ServerProxy( url )
		elif port is not None:
			assert isinstance( port	, int ) or isinstance( port	, long )
			self.rpcserver	= xmlrpclib.ServerProxy( 'http://127.0.0.1:' + str( port ) + '/neoip_lib/RPC2' )
		else:
			assert 0
		
		# open the session with the server
		self.sessid	= self.rpcserver.dht.session_start( self.realmid.to_xmlrpc(), self.nodeid.to_xmlrpc()
									, [["nb_replication", "5"], ["cookie_store_ok", "true"]] )
		# TODO here is a kad_profile hardcoded - do a class of it

############################################################################
############################################################################
############################################################################

	def publish( self, record, opttl = kad_delay( 30 ) ):
		'''Publish a record on this session.
			
			It return 0 on success, in case of error it trigger an exception.	
		'''
		# sanity check - check the parameters type
		assert isinstance( record, kad_rec )
		assert isinstance( opttl, kad_delay )
		# send the rpc itself
		return self.rpcserver.dht.publish( record.to_xmlrpc(), self.sessid, opttl.to_xmlrpc() )

############################################################################
############################################################################
############################################################################

	def delete( self, recid, keyid, opttl = kad_delay( 30 ) ):
		'''Delete a record on this session.
	
				It return 0 on success, in case of error it trigger an exception.	
		'''
		# sanity check - check the parameters type
		assert isinstance( recid, kad_id )
		assert isinstance( keyid, kad_id )
		assert isinstance( opttl, kad_delay )
		# send the rpc itself
		return self.rpcserver.dht.delete( recid.to_xmlrpc(), keyid.to_xmlrpc(), self.sessid, opttl.to_xmlrpc() )
		
############################################################################
############################################################################
############################################################################

	def getsome( self, keyid, max_nb_record = 1, opttl = kad_delay( 30 ) ):
		'''Get some records on this session (aka return what is available and return as soon as one is found)
		
			Dont return more than max_nb_record
		
			It return a list of 2 elements
			  - Element 1 is bool equal to True, the destination node has more records matching
			    this keyid
			  - Element 2 is a list of the found kad_rec		
		'''
		# sanity check - check the parameters type
		assert isinstance( keyid, kad_id )
		assert isinstance( opttl, kad_delay )
		# send the rpc itself
		result = self.rpcserver.dht.getsome( keyid.to_xmlrpc(), str( max_nb_record ), self.sessid, opttl.to_xmlrpc() )
		# convert the result and return it
		return [result[0], [kad_rec.from_xmlrpc( i ) for i in result[1]]]
		
############################################################################
############################################################################
############################################################################

	def getall( self, keyid, max_nb_record = 1, opttl = kad_delay( 30 ) ):
		'''Get all records on this session

			Dont return more than max_nb_record

			It return a list of 2 elements
			  - Element 1 is bool equal to True, the destination node has more records matching
			    this keyid
			  - Element 2 is a list of the found kad_rec
		'''
		# sanity check - check the parameters type
		assert isinstance( keyid	, kad_id )
		assert isinstance( opttl	, kad_delay )
		# send the rpc itself
		result = self.rpcserver.dht.getall( keyid.to_xmlrpc(), str( max_nb_record ), self.sessid, opttl.to_xmlrpc() )
		# convert the result and return it
		return [result[0], [kad_rec.from_xmlrpc( i ) for i in result[1]]]			

		
############################################################################
############################################################################
############################################################################

	def	nsearch( self, targetid, opttl = kad_delay( 30 ) ):
		'''Perform a nsearch on this session.
		
			It return a list of kad_naddrc the closest to the target_id
		'''
		# sanity check - check the parameters type
		assert isinstance( targetid	, kad_id )
		assert isinstance( opttl	, kad_delay )
		# send the rpc itself
		result = self.rpcserver.dht.nsearch( targetid.to_xmlrpc(), self.sessid, opttl.to_xmlrpc() )
		# convert the result and return it
		return [kad_naddrc.from_xmlrpc( i ) for i in result]

############################################################################
############################################################################
############################################################################

	def	ping_rpc( self, dest_naddr, opttl = kad_delay( 30 ) ):
		'''Perform a PING rpc to dest_naddr.

			It return 0 on success, in case of error it trigger an exception,
		'''
		# sanity check - check the parameters type
		assert isinstance( dest_naddr	, kad_naddr )
		assert isinstance( opttl		, kad_delay )
		# send the rpc itself
		return self.rpcserver.dht.ping_rpc( dest_naddr.to_xmlrpc(), self.sessid, opttl.to_xmlrpc() )

############################################################################
############################################################################
############################################################################

	def	store_rpc( self, recdups, cookie_id, dest_naddr, opttl = kad_delay( 30 ) ):
		'''Perform a STORE rpc with recdups using cookie_id to dest_naddr.
		
			It return 0 on success, in case of error it trigger an exception.
		'''
		# sanity check - check the parameters type
		assert isinstance( recdups		, list )
		assert isinstance( len( recdups ) > 0 )
		assert isinstance( recdups[0]	, kad_rec )
		assert isinstance( cookie_str	, str )
		assert isinstance( dest_naddr	, kad_naddr )
		assert isinstance( opttl		, kad_delay )
		# send the rpc itself
		return self.rpcserver.dht.store_rpc( [kad_rec.to_xmlrpc( i ) for i in recdups], dest_naddr.to_xmlrpc()
														, self.sessid, opttl.to_xmlrpc() ) 

############################################################################
############################################################################
############################################################################

	def	findnode_rpc( self, nodeid, dest_naddr, opttl = kad_delay( 30 ) ):
		'''Perform a FINDNODE rpc for nodeid to dest_naddr
		
			It return a list of 2 elements:
				  - Element 1 is a string equal to a cookie_id
				  - Element 2 is a list of the nclosest kad_naddr to the keyid
		'''
		# sanity check - check the parameters type
		assert isinstance( nodeid		, kad_id )
		assert isinstance( dest_naddr	, kad_naddr )
		assert isinstance( opttl		, kad_delay )
		# send the rpc itself
		result = self.rpcserver.dht.findnode_rpc( nodeid.to_xmlrpc(), dest_naddr.to_xmlrpc(), self.sessid, opttl.to_xmlrpc() )
		# convert and return the result
		return [result[0], [kad_naddr.from_xmlrpc( i ) for i in result[1]]]

############################################################################
############################################################################
############################################################################

	def	findsomeval_rpc( self, keyid, max_nb_record, dest_naddr, opttl = kad_delay( 30 ) ):
		'''Perform a FINDSOMEVAL rpc to dest_naddr
		
			It may return 2 kind of results:
				- if the destination node has records matching this keyid, it return
				  a list of 2 elements
				  - Element 1 is bool equal to True, the destination node has more records matching
				    this keyid
				  - Element 2 is a list of the found kad_rec
				- if the destination node has no records matching this keyid, it return
				  a list of 2 elements
				  - Element 1 is a string equal to a cookie_id
				  - Element 2 is a list of the nclosest node to the keyid
		'''
		# sanity check - check the parameters type
		assert isinstance( keyid		, kad_id )
		assert isinstance( max_nb_record, int )
		assert isinstance( dest_naddr	, kad_naddr )
		assert isinstance( opttl		, kad_delay )
		# send the rpc itself
		result = self.rpcserver.dht.findsomeval_rpc( keyid.to_xmlrpc(), str( max_nb_record ), self.sessid, opttl.to_xmlrpc() )
		if isinstance(result[0], bool):
			# if it is a recdups
			return [result[0], [kad_rec.from_xmlrpc( i ) for i in result[1]]]			
		else:
			# if it is a naddrlist
			return [result[0], [kad_naddr.from_xmlrpc( i ) for i in result[1]]]	

############################################################################
############################################################################
############################################################################

	def	findallval_rpc( self, recid, keyid, keyid_ge, max_nb_record, dest_naddr, opttl = kad_delay( 30 ) ):
		'''Perform a FINDALLVAL rpc to dest_naddr

			It return a list of 2 elements
			  - Element 1 is bool equal to True, the destination node has more records matching
			    this keyid
			  - Element 2 is a list of the found kad_rec
		'''
		# sanity check - check the parameters type
		assert isinstance( recid		, kad_id )
		assert isinstance( keyid		, kad_id )
		assert isinstance( keyid_ge		, bool )
		assert isinstance( max_nb_record, int )
		assert isinstance( dest_naddr	, kad_naddr )
		assert isinstance( opttl		, kad_delay )
		# send the rpc itself
		result = self.rpcserver.dht.findsomeval_rpc( recid.to_xmlrpc(), keyid.to_xmlrpc(), keyid_ge
												, str( max_nb_record ), self.sessid, opttl.to_xmlrpc() )
		# convert the result
		return [result[0], [kad_rec.from_xmlrpc( i ) for i in result[1]]]	


############################################################################
############################################################################
############################################################################

	def	delete_rpc( self, recid, cookie_id, dest_naddr, opttl = kad_delay( 30 ) ):
		'''Perform a DELETE rpc to delete the record recid using cookie_id in dest_naddr
		
			It return 0 on success, in case of error it trigger an exception
		'''
		# sanity check - check the parameters type
		assert isinstance( recid		, kad_id )
		assert isinstance( cookie_id	, str )
		assert isinstance( dest_naddr	, kad_naddr )
		assert isinstance( opttl		, kad_delay )
		# send the rpc itself
		return self.rpcserver.dht.delete_rpc( recid.to_xmlrpc(), cookie_id, self.sessid, opttl.to_xmlrpc() )


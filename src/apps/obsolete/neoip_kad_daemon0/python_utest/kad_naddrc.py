from kad_id		import kad_id
from kad_naddr	import kad_naddr

class kad_naddrc:
	'''Class to contain a kad_naddrc aka ip:port + nodeid + cookie_id'''
	
	def __init__( self, *args ):
		'''Constructor by default'''
		# copy the parameter
		if len( args ) == 1 and isinstance( args[0], kad_naddrc ):
			self.addr		= args[0].addr
			self.nodeid		= args[0].nodeid
			self.cookie_id	= args[0].cookie_id
		elif len( args ) == 3:
			self.addr		= args[0]
			self.nodeid		= args[1]
			self.cookie_id	= args[2]		
		else:
			assert 0
		# sanity check - check the parameter type
		assert isinstance( self.addr		, str )
		assert isinstance( self.nodeid		, kad_id )
		assert isinstance( self.cookie_id	, str )

	def __repr__( self ):
		'''Compute the formal string representation'''
		return repr( [ self.addr , self.nodeid, self.cookie_id ] )

	def to_kad_naddr( self ):
		'''Convert a kad_naddrc into a kad_naddr'''
		return kad_naddr( self.addr, self.nodeid )

	def to_xmlrpc( self ):
		'''Convert the object to be sent thru a xmlrpc'''
		return [ self.addr, self.nodeid.to_xmlrpc(), self.cookie_id ]

	def from_xmlrpc( *args ):
		'''Convert the object just received from a xmlrpc'''
		assert isinstance( args[0], list )
		assert len( args[0] ) == 3
		return kad_naddrc( args[0][0], kad_id.from_xmlrpc( args[0][1] ), args[0][2] )
	from_xmlrpc	= staticmethod( from_xmlrpc )


		
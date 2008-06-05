from kad_id import kad_id

class kad_naddr:
	'''Class to contain a kad_naddr aka ip:port + nodeid'''
	
	def __init__( self, *args ):
		'''Constructor by default'''
		# copy the parameter
		if len( args ) == 1 and isinstance( args[0], kad_naddr ):
			self.addr		= args[0].addr
			self.nodeid		= args[0].nodeid
		elif len( args ) == 2:
			self.addr		= args[0]
			self.nodeid		= args[1]
		else:
			assert 0
		# sanity check - check the parameter type
		assert isinstance( self.addr		, str )
		assert isinstance( self.nodeid		, kad_id )

	def __repr__( self ):
		'''Compute the formal string representation'''
		return repr( [ self.addr , self.nodeid] )

	def to_xmlrpc( self ):
		'''Convert the object to be sent thru a xmlrpc'''
		return [ self.addr, self.nodeid.to_xmlrpc() ]

	def from_xmlrpc( *args ):
		'''Convert the object just received from a xmlrpc'''
		assert isinstance( args[0], list )
		assert len( args[0] ) == 2
		return kad_naddr( args[0][0], kad_id.from_xmlrpc( args[0][1] ) )
	from_xmlrpc	= staticmethod( from_xmlrpc )


		
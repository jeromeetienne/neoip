class kad_delay:
	'''Class to contain a kad_delay'''
	
	def __init__( self, delay ):
		'''Constructor'''
		if isinstance( delay, kad_delay ):
			# if the parameter is another kad_delay
			self.delay	= delay.delay
		elif isinstance( delay, int ) or isinstance( delay, long ) or isinstance( delay, str ):
			# if the parameter is an int or a long
			self.delay	= long( delay )
		else:
			assert 0

	def to_xmlrpc( self ):
		'''Convert the object to be sent thru a xmlrpc'''
		return str(self.delay)

	def from_xmlrpc( str ):
		'''Convert the object just received from a xmlrpc'''
		return kad_delay( str )
	from_xmlrpc	= staticmethod( from_xmlrpc )

	def __repr__( self ):
		'''Compute the formal string representation'''
		return str( self.delay )

		
		
# system import
import base64

# local import
from kad_id		import kad_id
from kad_delay	import kad_delay

class kad_rec:
	'''Class to contain a kad_rec'''
	
	def __init__( self, args = [] ):
		'''Constructor'''
		assert len( args ) == 4
		self.recid	= kad_id( args[0] )
		self.keyid	= kad_id( args[1] )
		assert isinstance( args[2]	, str )
		self.data	= args[2]
		self.ttl	= kad_delay( args[3] )

	def to_xmlrpc( self ):
		'''Convert the object to be sent thru a xmlrpc'''
		return [self.recid.to_xmlrpc(), self.keyid.to_xmlrpc(), base64.b64encode( self.data ), self.ttl.to_xmlrpc()]

	def from_xmlrpc( *args ):
		'''Convert the object just received from a xmlrpc'''
		assert isinstance( args[0], list )
		assert len( args[0] ) == 4
		return kad_rec( [kad_id.from_xmlrpc(args[0][0]), kad_id.from_xmlrpc(args[0][1])
								, base64.b64decode(args[0][2]), kad_delay.from_xmlrpc(args[0][3])] )		
	from_xmlrpc	= staticmethod( from_xmlrpc )

	def to_list( self ):
		return [self.recid, self.keyid, self.data, self.ttl]

	def __repr__( self ):
		return str( self.to_list() )
	
